/*
In Pyxel create a map with size that is multiples of the GB screen size 20 wide by 18 high
the topmost layer should be the map data
add two more layers:
- "objects" for placing things like NPCs that activate when you bump them...

There should be no more than 16 tiles
- at the moment we have to include graphics for events
  but maybe later we can "export tiles as separate images" and export
  16+ tiles, with those having indexes higher than 16 being references to
  objects, defined separately...
  OR we can just use indexes higher than 16 to refer to objects... hmm

export "tilemap" as json
export "tileset" as png
*/

const IN_DIR = "./assets/prototype";
const OUT_DIR = "./src/includes/maps";

const fs = require("fs");
const path = require("path");

const jsonsInDir = fs
  .readdirSync(IN_DIR)
  .filter((file) => path.extname(file) === ".json");

jsonsInDir.forEach((file) => {
  const fileData = fs.readFileSync(path.join(IN_DIR, file));
  const json = JSON.parse(fileData.toString());

  // validate width, height is a multiple of 18x20
  assert(json.tileshigh % 18 == 0, "tileshigh is not a mulitple of 18");
  assert(json.tileswide % 20 == 0, "tileswide is not a mulitple of 20");

  const tilemapLayer = json.layers[0];
  assert(
    tilemapLayer.name !== "objects",
    "objects layer must not be first layer!"
  );

  const content = json.layers[0].tiles
    .reduce(getSubmapReducer(20, 18, json), [])
    .reduce((acc, rowOfSubmaps, y) => {
      acc.push(
        rowOfSubmaps.map((singleSubmap, x) => {
          const id = file.split(".")[0];
          return toMapBuilderInput(singleSubmap, id, x, y);
        })
      );
      return acc;
    }, [])
    .flat();

  // write tilemap data
  content.forEach((mapInclude) => {
    fs.writeFileSync(
      `${OUT_DIR}/${mapInclude.filename}`,
      build(mapInclude).trim()
    );
  });
});

function build(mapObject) {
  return `
IF !DEF(${mapObject.include}_INC)
DEF ${mapObject.include}_INC EQU 1

${mapObject.name}TileMap:
  ; db ${mapObject.height}, ${mapObject.width} -- all maps are 18w x 20h
${mapObject.content}
${mapObject.name}TileMapEnd:

initTileMapObjects:
${mapObject.objects}
  ret

ENDC
  `;
}

function assert(pred, message) {
  if (!pred) {
    throw new Error(`assertion failed: ${message}`);
  }
}

function toIncludeName(str, y, x) {
  return `${str.toUpperCase().replace(/-/g, "_")}_Y${y}_X${x}`;
}

function toMapName(str, y, x) {
  const [first, ...rest] = str.split("");
  return `${first.toUpperCase()}${camelize(rest.join(""))}Y${y}X${x}`;
}

function camelize(str) {
  return str
    .toLowerCase()
    .replace(/[^a-zA-Z0-9]+(.)/g, (m, chr) => chr.toUpperCase());
}

function toHex(n) {
  return `$${n.toString(16).toUpperCase().padStart(2, "0")}`;
}

function getSubmapReducer(width, height, json) {
  // grid of maps
  const objectsLayer = json.layers.find((layer) => layer.name == "objects");

  return function assignTileToSubmap(submaps, tile) {
    // determine which submap this tile goes into with y, x and modulo
    const i = (tile.x / width) | 0;
    const j = (tile.y / height) | 0;
    const x = tile.x % width;
    const y = tile.y % height;

    if (!submaps[j]) submaps[j] = [];
    if (!submaps[j][i]) submaps[j][i] = [];
    if (!submaps[j][i][y]) submaps[j][i][y] = [];

    submaps[j][i][y][x] = {
      x,
      y,
      tile: tile.tile,
      object: objectsLayer.tiles[tile.x + tile.y * json.tileswide].tile,
    };

    return submaps;
  };
}

function toMapBuilderInput(rawMapData, id, x, y) {
  return {
    filename: `${id}-y${y}-x${x}-data.asm`,
    include: toIncludeName(id, y, x), // TO_SNAKE_CONSTANT_CASE
    name: toMapName(id, y, x), // toCamelCase
    height: 18,
    width: 20,
    content: rawMapData
      .map((mapRow) => {
        return `  db ${mapRow
          .map(({ tile }) => tile)
          .map(toHex)
          .join(", ")}`;
      })
      .join("\n"),
    objects: rawMapData
      .map((mapRow) => {
        return mapRow
          .filter(({ object }) => object !== -1)
          .map(
            ({ x, y, object }) =>
              `  ld b, ${y}\n  ld c, ${x}\n  ld a, ${toHex(
                object
              )}\n  call initMapObject\n`
          )
          .join("\n");
      })
      .filter((mapRow) => mapRow.length)
      .join("\n"),
  };
}

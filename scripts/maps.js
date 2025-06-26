console.log(process.argc, process.argv);

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
  console.log(json);

  // validate width, height is a multiple of 18x20
  assert(json.tileshigh % 18 == 0, "tileshigh is not a mulitple of 18");
  assert(json.tileswide % 20 == 0, "tileswide is not a mulitple of 20");

  // grid of maps

  const content = json.layers[0].tiles
    .reduce((acc, tile) => {
      // determine which submap this tile goes into with y, x and modulo
      const i = (tile.x / 20) | 0;
      const j = (tile.y / 18) | 0;
      const x = tile.x % 20;
      const y = tile.y % 18;

      if (!acc[j]) acc[j] = [];
      if (!acc[j][i]) acc[j][i] = [];
      if (!acc[j][i][y]) acc[j][i][y] = [];

      acc[j][i][y][x] = tile.tile;

      return acc;
    }, [])
    .reduce((acc, rowOfMaps, y) => {
      acc.push(
        rowOfMaps.map((singleMap, x) => {
          const id = file.split(".")[0];
          return {
            filename: `${id}-y${y}-x${x}.inc`,
            include: toIncludeName(id, y, x), // TO_SNAKE_CONSTANT_CASE
            name: toMapName(id, y, x), // toCamelCase
            height: 18,
            width: 20,
            content: singleMap
              .map((mapRow) => {
                return `  db ${mapRow.map(toHex).join(", ")}`;
              })
              .join("\n"),
          };
        })
      );

      return acc;
    }, [])
    .flat();

  content.forEach((mapInclude) => {
    console.log(build(mapInclude));
    fs.writeFileSync(`${OUT_DIR}/${mapInclude.filename}`, build(mapInclude).trim());

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
  const [first, ...rest] = str.split('');
  return `${first.toUpperCase()}${camelize(rest.join(''))}Y${y}X${x}`;
}

function camelize(str) {
  return str
    .toLowerCase()
    .replace(/[^a-zA-Z0-9]+(.)/g, (m, chr) => chr.toUpperCase());
}

function toHex(n) {
  return `$${n.toString(16).toUpperCase().padStart(2, '0')}`
}
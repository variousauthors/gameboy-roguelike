

/**
 * 
 * ####
 * #  #
 * #--#
 * _#_#
 * 
 */


const map = `
TTTTT.~....TTT.TTTTT
TTTTT.~...TTTT.TTTTT
TTTTT.~...T.TT.TTTTT
TTTTT.~...TTTT.TTTTT
TTTT..~..TTTTT.TTTTT
....................
TTTT.~...TTTTT.TTTTT
TTT..~..TTTTTT.TTTTT
TTT.~~..TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
TTT.~...TTTTTT.TTTTT
`;

const TOKEN_HASH = "#".charCodeAt(0);
const TOKEN_DOT = ".".charCodeAt(0);
const TOKEN_TREE = "T".charCodeAt(0);
const TOKEN_WATER = "~".charCodeAt(0);

const result = map
  .split('\n')
  .slice(1, -1)
  .map((line) => {
    const db = line.split('').map((char) => char.charCodeAt(0)).map((char) => {
      switch (char) {
        case TOKEN_HASH:
          return "$01";
        case TOKEN_DOT:
          return "$00";
        case TOKEN_TREE:
          return "$02";
        case TOKEN_WATER:
          return "$03";
      }
    }).join(', ')

    return `  db ${db}, 0,0,0,0,0,0,0,0,0,0,0,0`
  }).join('\n')


console.log(result)
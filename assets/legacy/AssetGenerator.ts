import type Phaser from 'phaser';

export interface PixelAssetDefinition {
  palette: Record<string, string>;
  pixels: string[];
}

export const ASSET_DEFINITIONS: Record<string, PixelAssetDefinition> = {
  grass: {
    palette: {
      '.': '#2d6a4f',
      'G': '#38b000',
      'g': '#1b4332',
      'h': '#70e000',
    },
    pixels: [
      'GGGGGGGGGGGGGGGG',
      'GGGhGGGGGGhGGGGG',
      'GGGGGGGGGGGGGGGG',
      'GGgGGGGGGgGGGGgG',
      'GGGGGGhGGGGGGGGG',
      'GGGGGGGGGGGGGhGG',
      'GgGGGGGGGGgGGGGG',
      'GGGGGGGGGGGGGGGG',
      'GGGhGGGGGGhGGGGG',
      'GGGGGGgGGGGGGGGG',
      'GGGGGGGGGGGGGGGG',
      'GgGGGGGGGGgGGgGG',
      'GGGGGGhGGGGGGGGG',
      'GGGGGGGGGGGGGhGG',
      'GGgGGGGGGgGGGGGG',
      'GGGGGGGGGGGGGGGG',
    ],
  },

  path: {
    palette: {
      '.': '#d4a373',
      'P': '#d4a373',
      'p': '#bc6c25',
      'q': '#e9edc9',
      'd': '#a3704c',
    },
    pixels: [
      'PPPPPPPPPPPPPPPP',
      'PPqPPPPPPpPPPPPP',
      'PPPPPdPPPPPPPPqP',
      'PPPpPPPPPPPdPPPP',
      'PPPPPPPPqPPPPPPP',
      'PdPPPPPPPPPPPPpP',
      'PPPPPPpPPPPPPPPP',
      'PPPPPPPPPdPPPPPP',
      'PPqPPPPPPPPPPqPP',
      'PPPPPdPPPPPPPPPP',
      'PPPpPPPPPPPdPPPP',
      'PPPPPPPPqPPPPPPP',
      'PdPPPPPPPPPPPPpP',
      'PPPPPPpPPPPPPPPP',
      'PPPPPPPPPdPPPPPP',
      'PPPPPPPPPPPPPPPP',
    ],
  },

  tree: {
    palette: {
      '.': '#00000000',
      'T': '#081c15',
      't': '#1b4332',
      'g': '#2d6a4f',
      'l': '#52b788',
      'b': '#582f0e',
      'B': '#331a04',
    },
    pixels: [
      '.....TTTTTT.....',
      '...TTllllgttTT..',
      '..TllgllltgtgtT.',
      '.TllgglltgtggttT',
      '.TlllgttgtgggtgT',
      'TllgggttgtgggttT',
      'TlgttttggttggttT',
      'TttttttggtttgtgT',
      'TTttttttgggggttT',
      '.TTttttggggttTT.',
      '..TTttgbbgtTT...',
      '....TbbbbbbT....',
      '....TbbbbbbT....',
      '....TbbbbbbT....',
      '....TBBBBBBT....',
      '.....TTTTTT.....',
    ],
  },

  water: {
    palette: {
      '.': '#0077b6',
      'W': '#0077b6',
      'w': '#0096c7',
      'f': '#caf0f8',
      'd': '#023e8a',
    },
    pixels: [
      'WWWWWWWWWWWWWWWW',
      'WWfffWWWWWWfffWW',
      'WfwwwfWWWWfwwwfW',
      'WWWWWWWWWWWWWWWW',
      'WWWWddWWWWWWddWW',
      'WWWWWWWWWWWWWWWW',
      'WWfffWWWWWWfffWW',
      'WfwwwfWWWWfwwwfW',
      'WWWWWWWWWWWWWWWW',
      'WWWWddWWWWWWddWW',
      'WWWWWWWWWWWWWWWW',
      'WWfffWWWWWWfffWW',
      'WfwwwfWWWWfwwwfW',
      'WWWWWWWWWWWWWWWW',
      'WWWWddWWWWWWddWW',
      'WWWWWWWWWWWWWWWW',
    ],
  },

  stone_wall: {
    palette: {
      '.': '#00000000',
      'M': '#1a202c',
      'S': '#4a5568',
      's': '#2d3748',
      'l': '#718096',
    },
    pixels: [
      'MMMMMMMMMMMMMMMM',
      'MlllSssMMlllSssM',
      'MllSSssMMllSSssM',
      'MllSSssMMllSSssM',
      'MMMMMMMMMMMMMMMM',
      'MSssMMlllSssMllS',
      'MSssMMllSSssMllS',
      'MSssMMllSSssMllS',
      'MMMMMMMMMMMMMMMM',
      'MlllSssMMlllSssM',
      'MllSSssMMllSSssM',
      'MllSSssMMllSSssM',
      'MMMMMMMMMMMMMMMM',
      'MSssMMlllSssMllS',
      'MSssMMllSSssMllS',
      'MMMMMMMMMMMMMMMM',
    ],
  },

  // Hero facing DOWN (idle)
  hero_down_0: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10', // Outline
      'H': '#6f4e37', // Hair
      'F': '#ffdfba', // Skin
      'E': '#1a202c', // Eyes
      'C': '#2d6a4f', // Cap / Tunic dark
      'c': '#52b788', // Tunic light
      'B': '#4a3525', // Belt / Boots
      'Y': '#ffd700', // Buckle
      'S': '#4a5568', // Shield
      's': '#2d3748',
    },
    pixels: [
      '.....OOOOOO.....',
      '....OHHHHHOO....',
      '...OHHHHHHHHO...',
      '...OFFEFFFEO....',
      '...OFFFFFFFO....',
      '...OFFEFFFEO....',
      '..OCCFFCFFCCO...',
      '.OCCccccccccCO..',
      '.OCccccccccccO..',
      '.OCccBOYBccScO..',
      '..OCBBBBBBBSO...',
      '...OCCCCCCCCSO..',
      '...OCCCCCCCCSO..',
      '...OOBBOOBBOO...',
      '...OHHHOOHHHO...',
      '....OOO...OOO...',
    ],
  },

  // Hero facing DOWN (walk step)
  hero_down_1: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'F': '#ffdfba',
      'E': '#1a202c',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'Y': '#ffd700',
      'S': '#4a5568',
      's': '#2d3748',
    },
    pixels: [
      '.....OOOOOO.....',
      '....OHHHHHOO....',
      '...OHHHHHHHHO...',
      '...OFFEFFFEO....',
      '...OFFFFFFFO....',
      '...OFFEFFFEO....',
      '..OCCFFCFFCCO...',
      '.OCCccccccccCO..',
      '.OCccccccccccO..',
      '.OCccBOYBccScO..',
      '..OCBBBBBBBSO...',
      '...OCCCCCCCCSO..',
      '..OOBBO..CCCSO..',
      '.OHHHOO..OBBO...',
      '..OOO....OHHHO..',
      '..........OOO...',
    ],
  },

  // Hero facing UP (idle)
  hero_up_0: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'S': '#4a5568',
      's': '#2d3748',
    },
    pixels: [
      '.....OOOOOO.....',
      '....OCCCCCOO....',
      '...OCCCCCCCCCO..',
      '...OccccccccCO..',
      '...OCCCCCCCCCO..',
      '...OHHHHHHHHO...',
      '..OCCCCCCCCCO...',
      '.OCccccccccccO..',
      '.OCccccccccccO..',
      '.OCccBBBBBBccO..',
      '..OCBBBBBBBBcO..',
      '...OCCCCCCCCCO..',
      '...OCCCCCCCCCO..',
      '...OOBBOOBBOO...',
      '...OHHHOOHHHO...',
      '....OOO...OOO...',
    ],
  },

  // Hero facing UP (walk step)
  hero_up_1: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'S': '#4a5568',
      's': '#2d3748',
    },
    pixels: [
      '.....OOOOOO.....',
      '....OCCCCCOO....',
      '...OCCCCCCCCCO..',
      '...OccccccccCO..',
      '...OCCCCCCCCCO..',
      '...OHHHHHHHHO...',
      '..OCCCCCCCCCO...',
      '.OCccccccccccO..',
      '.OCccccccccccO..',
      '.OCccBBBBBBccO..',
      '..OCBBBBBBBBcO..',
      '...OCCCCCCCCCO..',
      '..OOBBO..CCCCO..',
      '.OHHHOO..OBBO...',
      '..OOO....OHHHO..',
      '..........OOO...',
    ],
  },

  // Hero facing LEFT (idle)
  hero_left_0: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'F': '#ffdfba',
      'E': '#1a202c',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'Y': '#ffd700',
      'S': '#4a5568',
      's': '#2d3748',
      'X': '#3182ce',
    },
    pixels: [
      '....OOOOOO......',
      '...OHHHHHO......',
      '..OFFEHHHOO.....',
      '..OFEFHHHHO.....',
      '..OFFFCCCCCO....',
      '..OFFCccccCO....',
      '.OSSSccccccCO...',
      'OSXSSccBYBccO...',
      'OSXSSccBBcccO...',
      '.OSSSCCCCCCCO...',
      '..OSSCcCCCCCO...',
      '...OCCCCCCCCO...',
      '...OOBBOOBBOO...',
      '...OHHHOOHHHO...',
      '....OOO...OOO...',
      '................',
    ],
  },

  // Hero facing LEFT (walk step)
  hero_left_1: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'F': '#ffdfba',
      'E': '#1a202c',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'Y': '#ffd700',
      'S': '#4a5568',
      's': '#2d3748',
      'X': '#3182ce',
    },
    pixels: [
      '....OOOOOO......',
      '...OHHHHHO......',
      '..OFFEHHHOO.....',
      '..OFEFHHHHO.....',
      '..OFFFCCCCCO....',
      '..OFFCccccCO....',
      '.OSSSccccccCO...',
      'OSXSSccBYBccO...',
      'OSXSSccBBcccO...',
      '.OSSSCCCCCCCO...',
      '..OSSCcCCCCCO...',
      '..OOBBO..CCCO...',
      '.OHHHOO..OBBO...',
      '..OOO....OHHHO..',
      '..........OOO...',
      '................',
    ],
  },

  // Hero facing RIGHT (idle)
  hero_right_0: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'F': '#ffdfba',
      'E': '#1a202c',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'Y': '#ffd700',
      'S': '#4a5568',
      's': '#2d3748',
      'X': '#3182ce',
    },
    pixels: [
      '......OOOOOO....',
      '......OHHHHHO...',
      '.....OOHHHEFFO..',
      '.....OHHHFEFFO..',
      '....OCCCCCFIFO..', // F
      '....OCccccCFFO..',
      '...OCccccccSSSO.',
      '...OccBYBccSSXSO',
      '...OcccBBccSSXSO',
      '...OCCCCCCSSSSO.',
      '...OCCCCCcSSO...',
      '...OCCCCCCCCO...',
      '...OOBBOOBBOO...',
      '...OHHHOOHHHO...',
      '....OOO...OOO...',
      '................',
    ],
  },

  // Hero facing RIGHT (walk step)
  hero_right_1: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'H': '#6f4e37',
      'F': '#ffdfba',
      'E': '#1a202c',
      'C': '#2d6a4f',
      'c': '#52b788',
      'B': '#4a3525',
      'Y': '#ffd700',
      'S': '#4a5568',
      's': '#2d3748',
      'X': '#3182ce',
    },
    pixels: [
      '......OOOOOO....',
      '......OHHHHHO...',
      '.....OOHHHEFFO..',
      '.....OHHHFEFFO..',
      '....OCCCCCFIFO..',
      '....OCccccCFFO..',
      '...OCccccccSSSO.',
      '...OccBYBccSSXSO',
      '...OcccBBccSSXSO',
      '...OCCCCCCSSSSO.',
      '...OCCCCCcSSO...',
      '...OCCC..OOBBO..',
      '...OBBO..OHHHOO.',
      '..OHHHO...OOO...',
      '...OOO..........',
      '................',
    ],
  },

  heart_full: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'R': '#e53e3e',
      'r': '#c53030',
      'W': '#ffffff',
    },
    pixels: [
      '................',
      '................',
      '...OOOO...OOOO..',
      '..ORRRRO.ORRRRO.',
      '.ORWWRRROORRRRRO',
      '.ORWRRRROORRRRRO',
      '.ORRRRRRRRRRRRRO',
      '.ORRRRRRRRRRRRRO',
      '..ORRRRRRRRRRRO.',
      '..OrrrrrrrrrrrO.',
      '...OrrrrrrrrrO..',
      '....OrrrrrrrO...',
      '.....OrrrrrO....',
      '......OrrrO.....',
      '.......OrO......',
      '........O.......',
    ],
  },

  heart_half: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'R': '#e53e3e',
      'r': '#c53030',
      'W': '#ffffff',
      'D': '#2d3748',
      'd': '#1a202c',
    },
    pixels: [
      '................',
      '................',
      '...OOOO...OOOO..',
      '..ORRRRO.ODDDDO.',
      '.ORWWRRROODDDDDO',
      '.ORWRRRROODDDDDO',
      '.ORRRRRROODDDDDO',
      '.ORRRRRROODDDDDO',
      '..ORRRRROODDDDDO',
      '..OrrrrrOOddddO.',
      '...OrrrrOOdddO..',
      '....OrrrOOddO...',
      '.....OrrOOdO....',
      '......OrOOO.....',
      '.......OrO......',
      '........O.......',
    ],
  },

  heart_empty: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'D': '#2d3748',
      'd': '#1a202c',
    },
    pixels: [
      '................',
      '................',
      '...OOOO...OOOO..',
      '..ODDDDO.ODDDDO.',
      '.ODDDDDDODDDDDDO',
      '.ODDDDDDODDDDDDO',
      '.ODDDDDDODDDDDDO',
      '.ODDDDDDODDDDDDO',
      '..ODDDDDODDDDDO.',
      '..OdddddOddddO..',
      '...OddddOdddO...',
      '....OdddOddO....',
      '.....OddOdO.....',
      '......OdOO......',
      '.......OdO......',
      '........O.......',
    ],
  },

  shrine_pedestal_sword: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'w': '#ffffff',
      'g': '#ffd700',
      'b': '#00f5d4',
      'S': '#1a202c',
      's': '#4a5568',
      'l': '#718096',
    },
    pixels: [
      '......OO........',
      '.....OwwO.......',
      '....OggggO......',
      '.....ObbO.......',
      '.....ObbO.......',
      '....ObbbbO......',
      '...OSssssSO.....',
      '..OSllllllSO....',
      '..OslsssslsO....',
      '..OssssssssO....',
      '..OslsssslsO....',
      '..OssssssssO....',
      '.OssssssssssO...',
      '.OssssssssssO...',
      '.OSSSSSSSSSSO...',
      '.OOOOOOOOOOOO...',
    ],
  },

  shrine_pedestal_empty: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'S': '#1a202c',
      's': '#4a5568',
      'l': '#718096',
    },
    pixels: [
      '................',
      '................',
      '................',
      '................',
      '................',
      '....OSssSO......',
      '...OSssssSO.....',
      '..OSllllllSO....',
      '..OslsssslsO....',
      '..OssssssssO....',
      '..OslsssslsO....',
      '..OssssssssO....',
      '.OssssssssssO...',
      '.OssssssssssO...',
      '.OSSSSSSSSSSO...',
      '.OOOOOOOOOOOO...',
    ],
  },

  slash_down: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'W': '#ffffff',
      'C': '#00f5d4',
      'B': '#00b4d8',
    },
    pixels: [
      '................',
      '................',
      '................',
      '..OOOOOOOOOOOO..',
      '.OWWWWWWWWWWWWO.',
      'OOWCCCCCCCCCCWOO',
      '.OOCCCBBBBCCCO..',
      '...OOBB..BBOO...',
      '....OOOOOOOO....',
      '................',
      '................',
      '................',
      '................',
      '................',
      '................',
      '................',
    ],
  },

  slash_up: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'W': '#ffffff',
      'C': '#00f5d4',
      'B': '#00b4d8',
    },
    pixels: [
      '................',
      '................',
      '................',
      '................',
      '................',
      '................',
      '................',
      '....OOOOOOOO....',
      '...OOBB..BBOO...',
      '.OOCCCBBBBCCCO..',
      'OOWCCCCCCCCCCWOO',
      '.OWWWWWWWWWWWWO.',
      '..OOOOOOOOOOOO..',
      '................',
      '................',
      '................',
    ],
  },

  slash_left: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'W': '#ffffff',
      'C': '#00f5d4',
      'B': '#00b4d8',
    },
    pixels: [
      '................',
      '..OOOO..........',
      '.OWWOO..........',
      'OWWCOO..........',
      'OWCCOO..........',
      'OWCCOO..........',
      'OWCCBO..........',
      'OWCCBO..........',
      'OWCCBO..........',
      'OWCCOO..........',
      'OWCCOO..........',
      'OWWCOO..........',
      '.OWWOO..........',
      '..OOOO..........',
      '................',
      '................',
    ],
  },

  slash_right: {
    palette: {
      '.': '#00000000',
      'O': '#0b0c10',
      'W': '#ffffff',
      'C': '#00f5d4',
      'B': '#00b4d8',
    },
    pixels: [
      '................',
      '..........OOOO..',
      '..........OOWWO.',
      '..........OOCWWO',
      '..........OOCWWO',
      '..........OOCWWO',
      '..........OBCWWO',
      '..........OBCWWO',
      '..........OBCWWO',
      '..........OOCWWO',
      '..........OOCWWO',
      '..........OOCWWO',
      '..........OOWWO.',
      '..........OOOO..',
      '................',
      '................',
    ],
  },
};

// Fix the typo in right frames 'I' -> 'F'
ASSET_DEFINITIONS.hero_right_0.pixels[4] = '....OCCCCCEFFO..';
ASSET_DEFINITIONS.hero_right_1.pixels[4] = '....OCCCCCEFFO..';

/**
 * Generates all procedural pixel art textures in the given Phaser Scene.
 */
export function generateGameTextures(scene: Phaser.Scene): void {
  for (const [key, def] of Object.entries(ASSET_DEFINITIONS)) {
    if (scene.textures.exists(key)) {
      continue;
    }

    const canvasTexture = scene.textures.createCanvas(key, 16, 16);
    if (!canvasTexture) continue;

    const ctx = canvasTexture.getContext();
    for (let y = 0; y < 16; y++) {
      const row = def.pixels[y];
      for (let x = 0; x < 16; x++) {
        const char = row[x];
        const color = def.palette[char];
        if (color && color !== '#00000000') {
          ctx.fillStyle = color;
          ctx.fillRect(x, y, 1, 1);
        }
      }
    }
    canvasTexture.refresh();
  }
}

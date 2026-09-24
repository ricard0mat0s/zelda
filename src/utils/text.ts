export const RETRO_TITLE_FONT = '"Press Start 2P", monospace';
export const RETRO_UI_FONT = '"Silkscreen", "Courier New", monospace';
export const RETRO_FONT = RETRO_TITLE_FONT;

export interface RetroTextStyleOptions {
  fontFamily?: string;
  fontSize?: string;
  fontStyle?: string;
  color?: string;
  align?: string;
  stroke?: string;
  strokeThickness?: number;
  wordWrap?: { width: number; useAdvancedWrap?: boolean };
}

export function createRetroTextStyle(options: RetroTextStyleOptions = {}) {
  return {
    fontFamily: RETRO_TITLE_FONT,
    resolution: 4,
    ...options,
  };
}

export function createRetroUITextStyle(options: RetroTextStyleOptions = {}) {
  return {
    fontFamily: RETRO_UI_FONT,
    resolution: 4,
    ...options,
  };
}

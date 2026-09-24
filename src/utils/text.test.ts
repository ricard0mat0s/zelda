import { describe, it, expect } from 'vitest';
import { RETRO_TITLE_FONT, RETRO_UI_FONT, createRetroTextStyle, createRetroUITextStyle } from './text';

describe('Retro Text Utility', () => {
  it('specifies the Press Start 2P font for titles and Silkscreen for UI/hints', () => {
    expect(RETRO_TITLE_FONT).toContain('Press Start 2P');
    expect(RETRO_UI_FONT).toContain('Silkscreen');
  });

  it('configures high resolution (4) to eliminate subpixel font blurring and antialiasing', () => {
    const titleStyle = createRetroTextStyle({ fontSize: '15px', color: '#ffd700' });
    expect(titleStyle.resolution).toBe(4);
    expect(titleStyle.fontFamily).toBe(RETRO_TITLE_FONT);

    const uiStyle = createRetroUITextStyle({ fontSize: '8px', color: '#ffffff' });
    expect(uiStyle.resolution).toBe(4);
    expect(uiStyle.fontFamily).toBe(RETRO_UI_FONT);
  });
});

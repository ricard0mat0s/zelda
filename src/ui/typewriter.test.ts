import { describe, it, expect } from 'vitest';
import { Typewriter } from './typewriter';

describe('Typewriter', () => {
  it('initializes with empty displayed text and incomplete status', () => {
    const tw = new Typewriter("IT'S DANGEROUS TO GO ALONE!\nTAKE THIS.", 30);
    expect(tw.getDisplayedText()).toBe('');
    expect(tw.isComplete()).toBe(false);
    expect(tw.getFullText()).toBe("IT'S DANGEROUS TO GO ALONE!\nTAKE THIS.");
  });

  it('advances displayed text as time elapses', () => {
    const tw = new Typewriter('HELLO WORLD', 50); // 1 char every 50ms
    tw.update(50);
    expect(tw.getDisplayedText()).toBe('H');
    expect(tw.isComplete()).toBe(false);

    tw.update(100); // 2 more chars
    expect(tw.getDisplayedText()).toBe('HEL');
    expect(tw.isComplete()).toBe(false);
  });

  it('marks complete when all characters have been revealed', () => {
    const tw = new Typewriter('ABC', 20);
    tw.update(60);
    expect(tw.getDisplayedText()).toBe('ABC');
    expect(tw.isComplete()).toBe(true);

    // Further updates do not crash or change text
    tw.update(100);
    expect(tw.getDisplayedText()).toBe('ABC');
    expect(tw.isComplete()).toBe(true);
  });

  it('skip() instantly reveals the entire message and completes', () => {
    const tw = new Typewriter('LONG MESSAGE REVEALED INSTANTLY', 50);
    tw.update(50);
    expect(tw.getDisplayedText()).toBe('L');
    expect(tw.isComplete()).toBe(false);

    tw.skip();
    expect(tw.getDisplayedText()).toBe('LONG MESSAGE REVEALED INSTANTLY');
    expect(tw.isComplete()).toBe(true);
  });

  it('can reset with new text', () => {
    const tw = new Typewriter('FIRST', 10);
    tw.skip();
    expect(tw.isComplete()).toBe(true);

    tw.reset('SECOND TEXT');
    expect(tw.getDisplayedText()).toBe('');
    expect(tw.isComplete()).toBe(false);
    expect(tw.getFullText()).toBe('SECOND TEXT');
  });
});

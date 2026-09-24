export type Direction = 'up' | 'down' | 'left' | 'right';

export interface Position {
  x: number;
  y: number;
}

export interface PlayerStats {
  maxHearts: number;
  currentHearts: number;
  hasSword: boolean;
  hasBow: boolean;
  keys: number;
}

export interface HeartDisplayUnit {
  type: 'full' | 'half' | 'empty';
}

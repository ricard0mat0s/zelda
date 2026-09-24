import { defineConfig } from 'vite';

export default defineConfig({
  base: './',
  build: {
    target: 'esnext',
    assetsInlineLimit: 0,
  },
  server: {
    port: 3000,
    open: false,
  }
});

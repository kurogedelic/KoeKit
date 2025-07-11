import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'KoeKit',
  description: 'High-Performance Audio Synthesis for RP2350A',
  
  // GitHub Pages deployment
  base: '/KoeKit/',
  
  // Theme configuration
  themeConfig: {
    // Site navigation
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Getting Started', link: '/getting-started' },
      { text: 'API Reference', link: '/api/' },
      { text: 'Examples', link: '/examples/' },
      { text: 'GitHub', link: 'https://github.com/kurogedelic/KoeKit' }
    ],

    // Sidebar configuration
    sidebar: {
      '/': [
        {
          text: 'Getting Started',
          items: [
            { text: 'Introduction', link: '/' },
            { text: 'Installation', link: '/getting-started' },
            { text: 'Quick Start', link: '/quick-start' },
            { text: 'Hardware Setup', link: '/hardware-setup' }
          ]
        },
        {
          text: 'Core Concepts',
          items: [
            { text: 'Audio Engine', link: '/concepts/audio-engine' },
            { text: 'Oscillators', link: '/concepts/oscillators' },
            { text: 'Wavetables', link: '/concepts/wavetables' },
            { text: 'Filters', link: '/concepts/filters' },
            { text: 'Envelopes', link: '/concepts/envelopes' }
          ]
        },
        {
          text: 'API Reference',
          items: [
            { text: 'Overview', link: '/api/' },
            { text: 'Core Classes', link: '/api/core-classes' },
            { text: 'Oscillators', link: '/api/oscillators' },
            { text: 'Filters', link: '/api/filters' },
            { text: 'Envelopes', link: '/api/envelopes' },
            { text: 'Wavetables', link: '/api/wavetables' },
            { text: 'Global Functions', link: '/api/global-functions' }
          ]
        },
        {
          text: 'Examples',
          items: [
            { text: 'Overview', link: '/examples/' },
            { text: 'Basic Examples', link: '/examples/basic' },
            { text: 'Filter Examples', link: '/examples/filters' },
            { text: 'Envelope Examples', link: '/examples/envelopes' },
            { text: 'Custom Wavetables', link: '/examples/custom-wavetables' },
            { text: 'Complete Synthesizers', link: '/examples/synthesizers' }
          ]
        },
        {
          text: 'Advanced Topics',
          items: [
            { text: 'Performance Optimization', link: '/advanced/performance' },
            { text: 'Memory Management', link: '/advanced/memory' },
            { text: 'Custom Modules', link: '/advanced/custom-modules' },
            { text: 'PIO Integration', link: '/advanced/pio' }
          ]
        }
      ]
    },

    // Social links
    socialLinks: [
      { icon: 'github', link: 'https://github.com/kurogedelic/KoeKit' }
    ],

    // Footer
    footer: {
      message: 'Released under the LGPL-3.0 License.',
      copyright: 'Copyright © 2024 Leo Kuroshita for Hugelton Instruments'
    },

    // Search
    search: {
      provider: 'local'
    },

    // Edit link
    editLink: {
      pattern: 'https://github.com/kurogedelic/KoeKit/edit/main/docs/:path',
      text: 'Edit this page on GitHub'
    },

    // Last updated
    lastUpdated: {
      text: 'Updated at',
      formatOptions: {
        dateStyle: 'full',
        timeStyle: 'medium'
      }
    }
  },

  // Markdown configuration
  markdown: {
    lineNumbers: true
  },

  // Head configuration for meta tags
  head: [
    ['link', { rel: 'icon', href: '/KoeKit/favicon.ico' }],
    ['meta', { name: 'theme-color', content: '#3c8772' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:locale', content: 'en' }],
    ['meta', { property: 'og:title', content: 'KoeKit | High-Performance Audio Synthesis for RP2350A' }],
    ['meta', { property: 'og:description', content: 'Real-time audio synthesis library optimized for RP2350A microcontrollers with modular oscillators, filters, and effects.' }],
    ['meta', { property: 'og:site_name', content: 'KoeKit' }],
    ['meta', { property: 'og:url', content: 'https://kurogedelic.github.io/KoeKit/' }]
  ]
})
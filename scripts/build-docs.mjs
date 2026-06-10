#!/usr/bin/env node
// build-docs.mjs — Renderiza los .md de docs/ a HTML con el tema oscuro del sitio.
// Uso: node scripts/build-docs.mjs <directorio>   (por defecto: _site/docs)
// Requiere: marked (npm install marked)

import { readdir, readFile, writeFile } from 'node:fs/promises';
import { join, basename } from 'node:path';
import { marked } from 'marked';

const targetDir = process.argv[2] || '_site/docs';

marked.setOptions({ gfm: true, breaks: false });

function escapeHtml(s) {
  return s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

// Título: primer "# Heading" del markdown, o el nombre del archivo.
function extractTitle(md, fallback) {
  const m = md.match(/^#\s+(.+?)\s*$/m);
  return m ? m[1].replace(/[*_`]/g, '') : fallback;
}

// Reescribe enlaces relativos *.md -> *.html (deja intactos http(s) y anclas).
function rewriteMdLinks(html) {
  return html.replace(/href="([^"#:]+)\.md(#[^"]*)?"/g, 'href="$1.html$2"');
}

function template(title, body) {
  return `<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>${escapeHtml(title)} · ClockWise XE1E</title>
  <link rel="icon" type="image/png" href="https://github.com/jnthas/clockwise/blob/gh-pages/static/images/clockwise_logo.png?raw=true">
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    :root {
      --bg: #0a0a0f; --bg-secondary: #12121a; --card: #1a1d2e;
      --text: #e8e8f0; --text-dim: #9090a8; --accent: #f0b800;
      --accent-hover: #ffc820; --border: rgba(255,255,255,0.08); --radius: 12px;
    }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: var(--bg); color: var(--text); line-height: 1.7;
    }
    a { color: var(--accent); text-decoration: none; }
    a:hover { color: var(--accent-hover); text-decoration: underline; }
    nav {
      position: sticky; top: 0; z-index: 100;
      background: rgba(10,10,15,0.92); backdrop-filter: blur(10px);
      border-bottom: 1px solid var(--border); padding: 14px 24px;
    }
    nav .inner {
      max-width: 860px; margin: 0 auto; display: flex;
      align-items: center; gap: 16px; justify-content: space-between;
    }
    nav a.home { color: var(--accent); font-weight: 700; }
    nav .doc-title { color: var(--text-dim); font-size: 0.9rem; }
    main {
      max-width: 860px; margin: 0 auto; padding: 48px 24px 96px;
    }
    h1, h2, h3, h4 { line-height: 1.3; margin: 1.6em 0 0.6em; }
    h1 { font-size: 2.1rem; color: var(--text); margin-top: 0;
         padding-bottom: 0.4em; border-bottom: 2px solid var(--border); }
    h2 { font-size: 1.55rem; color: var(--accent); }
    h3 { font-size: 1.2rem; color: var(--text); }
    p, ul, ol { margin: 0.9em 0; color: #cfcfe0; }
    ul, ol { padding-left: 1.6em; }
    li { margin: 0.3em 0; }
    strong { color: var(--text); }
    code {
      background: rgba(255,255,255,0.07); padding: 2px 6px;
      border-radius: 5px; font-size: 0.88em;
      font-family: 'SFMono-Regular', Consolas, 'Liberation Mono', monospace;
      color: #f0b800;
    }
    pre {
      background: #11131f; border: 1px solid var(--border);
      border-radius: var(--radius); padding: 16px 18px; overflow-x: auto; margin: 1.2em 0;
    }
    pre code { background: none; padding: 0; color: #e8e8f0; }
    blockquote {
      border-left: 3px solid var(--accent); background: rgba(240,184,0,0.06);
      padding: 8px 18px; margin: 1.2em 0; color: var(--text-dim); border-radius: 0 8px 8px 0;
    }
    table {
      width: 100%; border-collapse: collapse; margin: 1.4em 0;
      background: var(--card); border-radius: var(--radius); overflow: hidden;
    }
    th, td { padding: 12px 16px; text-align: left; border-bottom: 1px solid var(--border); }
    th { background: rgba(240,184,0,0.1); color: var(--accent); font-weight: 600; }
    td { color: var(--text-dim); }
    img { max-width: 100%; border-radius: 8px; }
    hr { border: none; border-top: 1px solid var(--border); margin: 2em 0; }
    .back { display: inline-block; margin-top: 48px; color: var(--text-dim); font-size: 0.9rem; }
  </style>
</head>
<body>
  <nav>
    <div class="inner">
      <a class="home" href="../">← ClockWise XE1E</a>
      <span class="doc-title">${escapeHtml(title)}</span>
    </div>
  </nav>
  <main>
${body}
    <a class="back" href="../">← Volver al inicio</a>
  </main>
</body>
</html>
`;
}

const entries = await readdir(targetDir, { withFileTypes: true });
const mdFiles = entries.filter(e => e.isFile() && e.name.endsWith('.md')).map(e => e.name);

let count = 0;
for (const file of mdFiles) {
  const md = await readFile(join(targetDir, file), 'utf8');
  const title = extractTitle(md, basename(file, '.md'));
  const body = rewriteMdLinks(marked.parse(md));
  const outName = file.replace(/\.md$/, '.html');
  await writeFile(join(targetDir, outName), template(title, body), 'utf8');
  count++;
  console.log(`  ✓ ${file} → ${outName}`);
}

console.log(`Renderizados ${count} documento(s) en ${targetDir}`);

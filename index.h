const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>ESP32 OscilloScope Pro</title>
    <link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Orbitron:wght@400;700;900&display=swap"
        rel="stylesheet">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        :root {
            --bg: #020609;
            --panel: #050d12;
            --panel2: #070f16;
            --border: #0d2030;
            --border2: #122838;
            --acc: #00bfff;
            --acc2: #00ff88;
            --bright: #c0e0f0;
            --text: #7aafc8;
        }

        html, body {
            height: 100%;
            background: var(--bg);
            color: var(--text);
            font-family: 'Share Tech Mono', monospace;
            overflow: hidden;
        }

        body::before {
            content: '';
            position: fixed;
            inset: 0;
            background: repeating-linear-gradient(0deg, transparent, transparent 3px, rgba(0,0,0,0.07) 3px, rgba(0,0,0,0.07) 4px);
            pointer-events: none;
            z-index: 9999;
        }

        header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 8px 16px;
            background: linear-gradient(180deg, #030c14, #020a10);
            border-bottom: 1px solid var(--border2);
            position: relative;
            overflow: hidden;
            flex-shrink: 0;
        }

        header::after {
            content: '';
            position: absolute;
            bottom: 0; left: 0; right: 0;
            height: 1px;
            background: linear-gradient(90deg, transparent, var(--acc) 50%, transparent);
            opacity: 0.5;
        }

        .brand { display: flex; align-items: center; gap: 10px; }
        .brand-icon {
            width: 28px; height: 28px;
            border: 1.5px solid var(--acc);
            border-radius: 4px;
            display: flex; align-items: center; justify-content: center;
            flex-shrink: 0;
        }
        .brand-name {
            font-family: 'Orbitron', monospace;
            font-weight: 700; font-size: 0.9rem;
            color: var(--acc); letter-spacing: 2px;
            text-shadow: 0 0 12px rgba(0,191,255,0.4);
        }
        .brand-sub { font-size: 0.52rem; color: var(--text); letter-spacing: 3px; opacity: 0.65; }

        .hdr-right { display: flex; align-items: center; gap: 12px; }
        .hdr-stat { text-align: center; }
        .hdr-stat-val { font-family: 'Orbitron', monospace; font-size: 0.78rem; font-weight: 700; color: var(--bright); }
        .hdr-stat-label { font-size: 0.48rem; color: var(--text); letter-spacing: 1.5px; opacity: 0.65; }

        .status-pill {
            display: flex; align-items: center; gap: 6px;
            background: var(--panel2);
            border: 1px solid var(--border2);
            border-radius: 20px;
            padding: 4px 10px;
            font-size: 0.62rem; letter-spacing: 1px;
        }

        .dot {
            width: 7px; height: 7px; border-radius: 50%;
            background: var(--acc2); box-shadow: 0 0 6px var(--acc2);
            animation: blink 1.4s ease-in-out infinite;
        }
        .dot.off { background: #ff4040; box-shadow: 0 0 6px #ff4040; animation: none; }

        @keyframes blink { 0%,100%{opacity:1} 50%{opacity:0.2} }

        .app { display: flex; flex-direction: column; height: calc(100vh - 48px); }
        .main-area { flex: 1; display: flex; min-height: 0; }
        .scope-col { flex: 1; display: flex; flex-direction: column; min-width: 0; }

        .scope-wrapper {
            flex: 1; position: relative;
            padding: 10px 10px 4px;
            min-height: 0;
        }

        canvas#scope {
            display: block; width: 100%; height: 100%;
            border-radius: 6px;
            border: 1px solid var(--border2);
            background: #020e07;
            cursor: crosshair;
            transition: border-color 0.15s;
        }

        /* Drag-pan active cursor */
        canvas#scope.panning { cursor: grabbing !important; border-color: rgba(0,191,255,0.5); }
        canvas#scope.pan-ready { cursor: grab; }

        .vignette {
            position: absolute; inset: 10px 10px 4px;
            border-radius: 6px;
            background: radial-gradient(ellipse at center, transparent 55%, rgba(0,0,0,0.6) 100%);
            pointer-events: none; z-index: 10;
        }

        .corner { position: absolute; width: 14px; height: 14px; z-index: 11; }
        .corner.tl { top:14px; left:14px; border-top:1.5px solid var(--acc); border-left:1.5px solid var(--acc); }
        .corner.tr { top:14px; right:14px; border-top:1.5px solid var(--acc); border-right:1.5px solid var(--acc); }
        .corner.bl { bottom:8px; left:14px; border-bottom:1.5px solid var(--acc); border-left:1.5px solid var(--acc); }
        .corner.br { bottom:8px; right:14px; border-bottom:1.5px solid var(--acc); border-right:1.5px solid var(--acc); }

        .cursor-ro {
            position: absolute; top: 16px; left: 20px; z-index: 20;
            background: rgba(2,10,16,0.92);
            border: 1px solid var(--border2);
            border-radius: 4px; padding: 4px 10px;
            font-size: 0.6rem; color: var(--text); letter-spacing: 1px;
            pointer-events: none; opacity: 0; transition: opacity 0.15s;
        }
        .cursor-ro.vis { opacity: 1; }
        .cursor-ro span { color: var(--bright); }

        /* ── PAN OVERLAY (bottom-right of canvas) ── */
        .pan-overlay {
            position: absolute;
            bottom: 18px; right: 18px;
            z-index: 20;
            display: flex; flex-direction: column; align-items: center; gap: 2px;
            opacity: 0.7;
            pointer-events: none;
            transition: opacity 0.3s;
        }
        .pan-overlay.active { opacity: 1; }
        .pan-row { display: flex; gap: 2px; }
        .pan-key {
            width: 22px; height: 22px;
            background: rgba(0,191,255,0.1);
            border: 1px solid rgba(0,191,255,0.3);
            border-radius: 3px;
            display: flex; align-items: center; justify-content: center;
            font-size: 10px; color: rgba(0,191,255,0.7);
            transition: all 0.1s;
            pointer-events: none;
        }
        .pan-key.lit {
            background: rgba(0,191,255,0.25);
            border-color: rgba(0,191,255,0.8);
            color: #00bfff;
            box-shadow: 0 0 6px rgba(0,191,255,0.4);
        }
        .pan-hint {
            font-size: 0.42rem; color: rgba(0,191,255,0.4);
            letter-spacing: 1px; margin-top: 2px;
            pointer-events: none;
        }

        /* Pan position indicator strip */
        .pan-indicator {
            position: absolute;
            bottom: 6px; left: 10px; right: 10px;
            height: 3px;
            z-index: 20;
            pointer-events: none;
        }
        .pan-indicator-track {
            width: 100%; height: 100%;
            background: rgba(0,191,255,0.08);
            border-radius: 2px;
            position: relative;
        }
        .pan-indicator-thumb {
            position: absolute;
            top: 0; height: 100%;
            background: rgba(0,191,255,0.5);
            border-radius: 2px;
            transition: left 0.05s, width 0.05s;
            box-shadow: 0 0 4px rgba(0,191,255,0.4);
        }

        /* ── RIGHT PANEL ── */
        .right-panel {
            width: 220px; flex-shrink: 0;
            background: var(--panel);
            border-left: 1px solid var(--border2);
            display: flex; flex-direction: column;
            overflow: hidden;
        }

        .rpanel-tab { display: flex; border-bottom: 1px solid var(--border2); }
        .rpanel-tab button {
            flex: 1; background: transparent; border: none;
            color: var(--text);
            font-family: 'Share Tech Mono', monospace;
            font-size: 0.58rem; letter-spacing: 1.5px;
            padding: 7px 4px; cursor: pointer;
            border-bottom: 2px solid transparent;
            transition: all 0.15s;
        }
        .rpanel-tab button.active { color: var(--acc); border-bottom-color: var(--acc); }

        .rpanel-body {
            flex: 1; overflow-y: auto;
            padding: 8px;
            scrollbar-width: thin;
            scrollbar-color: var(--border2) transparent;
        }

        .meas-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 5px; }
        .mitem {
            background: var(--panel2);
            border: 1px solid var(--border2);
            border-radius: 4px; padding: 5px 6px;
        }
        .mitem-label { font-size: 0.48rem; letter-spacing: 1.5px; color: var(--text); opacity: 0.6; margin-bottom: 2px; }
        .mitem-val { font-family: 'Orbitron', monospace; font-size: 0.68rem; font-weight: 700; color: var(--acc); }

        .section-title {
            font-size: 0.5rem; letter-spacing: 2.5px;
            color: var(--text); opacity: 0.5;
            padding: 6px 0 4px;
            border-top: 1px solid var(--border2);
            margin-top: 6px;
        }
        .section-title:first-child { border-top: none; margin-top: 0; }

        .cursor-row { display: flex; align-items: center; gap: 6px; margin-bottom: 5px; }
        .cursor-toggle { display: flex; align-items: center; gap: 4px; font-size: 0.55rem; color: var(--text); }
        .cursor-delta { background: var(--panel2); border: 1px solid var(--border2); border-radius: 4px; padding: 6px 8px; margin-top: 8px; }
        .cursor-delta-row { display: flex; justify-content: space-between; margin-bottom: 3px; }
        .cdlabel { font-size: 0.5rem; color: var(--text); opacity: 0.65; }
        .cdval { font-family: 'Orbitron', monospace; font-size: 0.65rem; font-weight: 700; color: var(--bright); }

        .stat-row { display: flex; justify-content: space-between; padding: 3px 0; border-bottom: 1px solid rgba(13,32,48,0.5); }
        .stat-label { font-size: 0.52rem; color: var(--text); opacity: 0.6; }
        .stat-val { font-family: 'Orbitron', monospace; font-size: 0.58rem; color: var(--bright); }

        /* ── PAN READOUT in right panel ── */
        .pan-readout {
            background: var(--panel2);
            border: 1px solid var(--border2);
            border-radius: 4px;
            padding: 6px 8px;
            margin-top: 6px;
        }
        .pan-readout-row { display: flex; justify-content: space-between; margin-bottom: 2px; }
        .pr-label { font-size: 0.48rem; color: var(--text); opacity: 0.6; }
        .pr-val { font-family: 'Orbitron', monospace; font-size: 0.6rem; color: var(--acc); }

        /* ── CONTROLS BAR ── */
        .controls-bar {
            display: flex; align-items: stretch; gap: 1px;
            background: var(--border);
            border-top: 1px solid var(--border2);
            flex-shrink: 0;
            overflow-x: auto;
            scrollbar-width: none;
        }
        .controls-bar::-webkit-scrollbar { display: none; }

        .cg { display: flex; align-items: center; background: var(--panel); flex-shrink: 0; }
        .cg+.cg { border-left: 1px solid var(--border2); }
        .clabel { font-size: 0.5rem; letter-spacing: 2px; color: var(--text); opacity: 0.55; padding: 0 8px; text-transform: uppercase; white-space: nowrap; }

        .run-btn {
            font-family: 'Orbitron', monospace; font-weight: 700;
            font-size: 0.65rem; letter-spacing: 2px;
            padding: 10px 14px; border: none; cursor: pointer;
            background: linear-gradient(180deg, #0a2a0f, #061808);
            color: var(--acc2);
            border-right: 1px solid var(--border2);
            transition: all 0.15s; white-space: nowrap;
            position: relative; overflow: hidden;
        }
        .run-btn.stopped { background: linear-gradient(180deg, #2a0a08, #180605); color: #ff5040; }
        .rbdot {
            display: inline-block; width: 7px; height: 7px; border-radius: 50%;
            background: var(--acc2); margin-right: 6px;
            box-shadow: 0 0 5px var(--acc2); animation: blink 1.4s infinite;
        }
        .run-btn.stopped .rbdot { background: #ff5040; box-shadow: 0 0 5px #ff5040; animation: none; }

        .mrow { display: flex; }
        .mbtn {
            font-family: 'Share Tech Mono', monospace; font-size: 0.58rem;
            letter-spacing: 1px; padding: 8px 10px;
            border: none; cursor: pointer;
            background: transparent; color: var(--text);
            border-right: 1px solid var(--border2);
            transition: all 0.15s; white-space: nowrap;
        }
        .mbtn:hover { background: rgba(0,191,255,0.06); color: var(--bright); }
        .mbtn.active { background: rgba(0,191,255,0.1); color: var(--acc); box-shadow: inset 0 -2px 0 var(--acc); }

        .sw { display: flex; align-items: center; gap: 7px; padding: 0 10px; }
        .slabel { font-size: 0.54rem; color: var(--text); opacity: 0.65; white-space: nowrap; }
        .sval { font-family: 'Orbitron', monospace; font-size: 0.58rem; font-weight: 700; color: var(--bright); min-width: 48px; text-align: right; white-space: nowrap; }

        input[type=range] {
            -webkit-appearance: none; appearance: none;
            width: 68px; height: 3px;
            background: var(--border2); border-radius: 2px;
            cursor: pointer; outline: none;
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 12px; height: 12px; border-radius: 50%;
            background: var(--acc);
            box-shadow: 0 0 4px rgba(0,191,255,0.5);
            cursor: pointer; transition: transform 0.1s;
        }

        select.ctrl-sel {
            background: var(--panel2); border: 1px solid var(--border2);
            color: var(--bright);
            font-family: 'Share Tech Mono', monospace;
            font-size: 0.58rem; padding: 3px 6px;
            border-radius: 3px; cursor: pointer; outline: none;
        }

        /* ── STATUS BAR ── */
        .bottom {
            display: flex; align-items: center; justify-content: space-between;
            padding: 4px 14px;
            background: var(--panel2);
            border-top: 1px solid var(--border);
            flex-shrink: 0; gap: 8px; flex-wrap: wrap;
        }
        .leg { display: flex; align-items: center; gap: 6px; font-size: 0.56rem; letter-spacing: 0.5px; }
        .legline { width: 22px; height: 2px; border-radius: 1px; flex-shrink: 0; background: var(--acc); box-shadow: 0 0 4px var(--acc); }
        .meas-group { display: flex; gap: 10px; flex-wrap: wrap; }
        .meas { display: flex; flex-direction: column; align-items: flex-end; }
        .mv { font-family: 'Orbitron', monospace; font-size: 0.65rem; font-weight: 700; color: var(--bright); }
        .ml { font-size: 0.44rem; color: var(--text); opacity: 0.58; letter-spacing: 1px; }
        .wsinfo { font-size: 0.48rem; color: var(--text); opacity: 0.4; letter-spacing: 1px; white-space: nowrap; }

        /* Pan position display in status bar */
        .pan-status {
            display: flex; align-items: center; gap: 6px;
            font-size: 0.5rem; color: var(--text);
            opacity: 0.6; letter-spacing: 1px;
            white-space: nowrap;
            transition: color 0.2s;
        }
        .pan-status.active { opacity: 1; color: var(--acc); }
        .pan-status-val { font-family: 'Orbitron', monospace; font-size: 0.55rem; color: var(--bright); }
    </style>
</head>

<body>

    <header>
        <div class="brand">
            <div class="brand-icon">
                <svg width="18" height="18" viewBox="0 0 20 20" fill="none">
                    <polyline points="1,14 4,14 6,4 8,16 10,8 12,14 14,14 16,6 18,14 19,14"
                        stroke="#00bfff" stroke-width="1.5" stroke-linejoin="round"/>
                </svg>
            </div>
            <div>
                <div class="brand-name">OSCILLOSCOPE</div>
                <div class="brand-sub">HARDWARE PROBES &middot; LIVE</div>
            </div>
        </div>
        <div class="hdr-right">
            <div class="hdr-stat"><div class="hdr-stat-val" id="hdrFreq">-- Hz</div><div class="hdr-stat-label">FREQUENCY</div></div>
            <div class="hdr-stat"><div class="hdr-stat-val" id="hdrPeriod">-- ms</div><div class="hdr-stat-label">PERIOD</div></div>
            <div class="hdr-stat"><div class="hdr-stat-val" id="hdrVpp">-- mV</div><div class="hdr-stat-label">VPP</div></div>
            <div class="hdr-stat"><div class="hdr-stat-val" id="hdrSps">0</div><div class="hdr-stat-label">SPS</div></div>
            <div class="hdr-stat"><div class="hdr-stat-val" id="hdrAcq">0</div><div class="hdr-stat-label">ACQS</div></div>
            <div class="status-pill">
                <div class="dot off" id="statusDot"></div>
                <span id="statusTxt">CONNECTING</span>
            </div>
        </div>
    </header>

    <div class="app">
        <div class="main-area">
            <div class="scope-col">
                <div class="scope-wrapper">
                    <canvas id="scope"></canvas>
                    <div class="vignette"></div>
                    <div class="corner tl"></div>
                    <div class="corner tr"></div>
                    <div class="corner bl"></div>
                    <div class="corner br"></div>
                    <div class="cursor-ro" id="cro">T:<span id="cT">0</span>ms &nbsp;V:<span id="cV">0</span>mV</div>

                    <!-- Arrow key hint overlay (bottom-right of canvas) -->
                    <div class="pan-overlay" id="panOverlay">
                        <div class="pan-row">
                            <div class="pan-key" id="pkUp">▲</div>
                        </div>
                        <div class="pan-row">
                            <div class="pan-key" id="pkLeft">◀</div>
                            <div class="pan-key" id="pkDown">▼</div>
                            <div class="pan-key" id="pkRight">▶</div>
                        </div>
                        <div class="pan-hint">DRAG or ARROWS to PAN</div>
                    </div>

                    <!-- Horizontal position indicator strip -->
                    <div class="pan-indicator" id="panIndicator">
                        <div class="pan-indicator-track">
                            <div class="pan-indicator-thumb" id="panThumb"></div>
                        </div>
                    </div>
                </div>

                <div class="controls-bar">
                    <div class="cg">
                        <button class="run-btn" id="runBtn" onclick="toggleRun()">
                            <span class="rbdot" id="rbdot"></span><span id="runTxt">RUNNING</span>
                        </button>
                    </div>
                    <div class="cg">
                        <button class="mbtn" id="singleBtn" onclick="doSingle()">SINGLE</button>
                    </div>

                    <div class="cg">
                        <div class="clabel" style="color:var(--bright);">PROBE</div>
                        <div class="sw">
                            <select class="ctrl-sel" id="probeSel" onchange="changeProbe(this.value)" style="border-color:var(--acc);">
                                <option value="ch1">PATH 1: LOGIC (3.3V)</option>
                                <option value="ch2">PATH 2: HIGH VOLT</option>
                                <option value="ch3">PATH 3: AC/AUDIO</option>
                            </select>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel">MODE</div>
                        <div class="mrow">
                            <button class="mbtn active" id="modeRoll" onclick="changeMode('roll')">ROLL</button>
                            <button class="mbtn" id="modeTrig" onclick="changeMode('trigger')">TRIG</button>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel">TRIGGER</div>
                        <div class="sw">
                            <span class="slabel">LVL</span>
                            <input type="range" min="0" max="3300" value="1650" step="10" id="trigSlider" oninput="updateTrigger(this.value)">
                            <span class="sval" id="trigText">1650mV</span>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel">EDGE</div>
                        <div class="mrow">
                            <button class="mbtn active" id="edgeRise" onclick="setEdge('rise')">&nearr;</button>
                            <button class="mbtn" id="edgeFall" onclick="setEdge('fall')">&searr;</button>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel">TIMEBASE</div>
                        <div class="sw">
                            <span class="slabel">DIV</span>
                            <input type="range" min="1" max="8" value="2" step="1" id="timeSlider" oninput="updateTimebase(this.value)">
                            <span class="sval" id="timeText">2x</span>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel" style="color:var(--acc);">PAN H</div>
                        <div class="sw">
                            <span class="slabel">X</span>
                            <input type="range" min="-500" max="500" value="0" step="1" id="hposSlider" oninput="updateHPos(this.value)">
                            <span class="sval" id="hposText">0px</span>
                        </div>
                        <button class="mbtn" onclick="resetHPos()" title="Reset horizontal position">⌂H</button>
                    </div>

                    <div class="cg">
                        <div class="clabel" style="color:var(--acc);">PAN V</div>
                        <div class="sw">
                            <span class="slabel">Y</span>
                            <input type="range" min="-1000" max="1000" value="0" step="10" id="ofsSlider" oninput="setOffset(this.value)">
                            <span class="sval" id="ofsText">0</span>
                        </div>
                        <button class="mbtn" onclick="resetVPos()" title="Reset vertical position">⌂V</button>
                    </div>

                    <div class="cg">
                        <div class="clabel" style="color:var(--acc);">TRACE</div>
                        <div class="sw">
                            <span class="slabel">GAIN</span>
                            <input type="range" min="0.2" max="4" value="1" step="0.1" id="gainSlider" oninput="setGain(this.value)">
                            <span class="sval" id="gainText">1.0x</span>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel">VIEW</div>
                        <div class="mrow">
                            <button class="mbtn active" id="viewTime" onclick="changeView('time')">YT</button>
                            <button class="mbtn" id="viewFft" onclick="changeView('fft')">FFT</button>
                        </div>
                    </div>

                    <div class="cg">
                        <div class="clabel">PERSIST</div>
                        <div class="mrow">
                            <button class="mbtn active" id="persOff" onclick="setPersist(0)">OFF</button>
                            <button class="mbtn" id="pers1" onclick="setPersist(0.92)">MED</button>
                            <button class="mbtn" id="pers2" onclick="setPersist(0.97)">INF</button>
                        </div>
                    </div>

                    <div class="cg">
                        <button class="mbtn" onclick="resetPan()" title="Reset all pan">⌂ RESET PAN</button>
                    </div>
                </div>
            </div>

            <div class="right-panel">
                <div class="rpanel-tab">
                    <button class="active" onclick="switchRPanel('meas',this)">MEAS</button>
                    <button onclick="switchRPanel('cursor',this)">CURSOR</button>
                    <button onclick="switchRPanel('stats',this)">STATS</button>
                </div>

                <div class="rpanel-body" id="rpanel-meas">
                    <div class="section-title">ACTIVE PROBE</div>
                    <div class="meas-grid">
                        <div class="mitem"><div class="mitem-label">VMAX</div><div class="mitem-val" id="mVmax1">--</div></div>
                        <div class="mitem"><div class="mitem-label">VMIN</div><div class="mitem-val" id="mVmin1">--</div></div>
                        <div class="mitem"><div class="mitem-label">VPP</div><div class="mitem-val" id="mVpp1">--</div></div>
                        <div class="mitem"><div class="mitem-label">VAVG</div><div class="mitem-val" id="mVavg1">--</div></div>
                        <div class="mitem"><div class="mitem-label">VRMS</div><div class="mitem-val" id="mVrms1">--</div></div>
                        <div class="mitem"><div class="mitem-label">DUTY%</div><div class="mitem-val" id="mDuty1">--</div></div>
                    </div>
                    <div class="section-title">TIMING</div>
                    <div class="meas-grid">
                        <div class="mitem"><div class="mitem-label">FREQ</div><div class="mitem-val" id="mFreq" style="color:var(--bright)">--</div></div>
                        <div class="mitem"><div class="mitem-label">PERIOD</div><div class="mitem-val" id="mPer" style="color:var(--bright)">--</div></div>
                    </div>
                    <div class="section-title">PAN POSITION</div>
                    <div class="pan-readout">
                        <div class="pan-readout-row"><span class="pr-label">H OFFSET</span><span class="pr-val" id="prHPos">0 px</span></div>
                        <div class="pan-readout-row"><span class="pr-label">V OFFSET</span><span class="pr-val" id="prVPos">0 mV</span></div>
                    </div>
                </div>

                <div class="rpanel-body" id="rpanel-cursor" style="display:none;">
                    <div class="section-title">CURSOR MODE</div>
                    <div class="cursor-row">
                        <label class="cursor-toggle"><input type="checkbox" id="curXEn" checked
                            onchange="setCursorMode(this.checked?(document.getElementById('curYEn').checked?'xy':'x'):'off')">
                            X cursors</label>
                    </div>
                    <div class="cursor-row">
                        <label class="cursor-toggle"><input type="checkbox" id="curYEn"
                            onchange="setCursorMode(this.checked?(document.getElementById('curXEn').checked?'xy':'y'):'off')">
                            Y cursors</label>
                    </div>
                    <div class="section-title">X CURSORS</div>
                    <div class="stat-row"><span class="stat-label">X1</span><span class="stat-val" id="cx1val">--</span></div>
                    <div class="stat-row"><span class="stat-label">X2</span><span class="stat-val" id="cx2val">--</span></div>
                    <div class="cursor-delta">
                        <div class="cursor-delta-row"><span class="cdlabel">&Delta;X (time)</span><span class="cdval" id="cDX">--</span></div>
                        <div class="cursor-delta-row"><span class="cdlabel">1/&Delta;X (freq)</span><span class="cdval" id="cDXF">--</span></div>
                    </div>
                    <div class="section-title">Y CURSORS</div>
                    <div class="stat-row"><span class="stat-label">Y1</span><span class="stat-val" id="cy1val">--</span></div>
                    <div class="stat-row"><span class="stat-label">Y2</span><span class="stat-val" id="cy2val">--</span></div>
                    <div class="cursor-delta">
                        <div class="cursor-delta-row"><span class="cdlabel">&Delta;Y (volt)</span><span class="cdval" id="cDY">--</span></div>
                    </div>
                </div>

                <div class="rpanel-body" id="rpanel-stats" style="display:none;">
                    <div class="section-title">ACQUISITION STATS</div>
                    <div class="stat-row"><span class="stat-label">Count</span><span class="stat-val" id="stAcq">0</span></div>
                    <div class="stat-row"><span class="stat-label">Mean Vavg</span><span class="stat-val" id="stMean">--</span></div>
                    <div class="stat-row"><span class="stat-label">Min Vpp</span><span class="stat-val" id="stMinVpp">--</span></div>
                    <div class="stat-row"><span class="stat-label">Max Vpp</span><span class="stat-val" id="stMaxVpp">--</span></div>
                    <button class="mbtn" style="margin-top:8px;width:100%;border:1px solid var(--border2);" onclick="clearStats()">CLEAR STATS</button>
                </div>
            </div>
        </div>

        <div class="bottom">
            <div style="display:flex;gap:14px;align-items:center;flex-wrap:wrap;">
                <div class="leg">
                    <div class="legline"></div>
                    <span style="color:var(--acc);" id="legTxt">PATH 1 (LOGIC)</span>
                </div>
            </div>
            <div class="meas-group">
                <div class="meas"><div class="mv" id="mVmax">-- mV</div><div class="ml">VMAX</div></div>
                <div class="meas"><div class="mv" id="mVmin">-- mV</div><div class="ml">VMIN</div></div>
                <div class="meas"><div class="mv" id="mVavg">-- mV</div><div class="ml">VAVG</div></div>
            </div>
            <!-- Pan status in bottom bar -->
            <div class="pan-status" id="panStatus">
                <span>PAN</span>
                <span>H:<span class="pan-status-val" id="psH">0</span></span>
                <span>V:<span class="pan-status-val" id="psV">0</span></span>
            </div>
            <div class="wsinfo" id="wsinfo">WS: connecting&hellip;</div>
        </div>
    </div>

    <script>
        var canvas = document.getElementById('scope');
        var ctx = canvas.getContext('2d');

        var bufSize = 2000;
        var traceData = new Float32Array(bufSize);
        var persistBuf = [];
        var persistAlpha = 0;
        var activeProbe = 'ch1';

        var timebase = 2, ptsDraw = 500;
        var triggered = false, ptIdx = 0;
        var trigLvl = 1650, lastV = 0;
        var trigEdge = 'rise';
        var running = true, singleMode = false, singleDone = false;
        var dispMode = 'roll', viewMode = 'time';
        var gain = 1.0, ofs = 0, hPos = 0;

        var spsC = 0, spsT = Date.now(), acqCount = 0;
        var recentTrace = [];
        var lastCross = null;

        var cursorMode = 'x';
        var curX1 = 0.25, curX2 = 0.75, curY1 = 0.3, curY2 = 0.7;
        var draggingCursor = null;
        var statsAcqCount = 0, statsVavgArr = [], statsVppArr = [], statsFreqArr = [];
        var mouseX, mouseY;

        // ── PAN STATE ──
        var panDragging = false;
        var panDragStartX = 0, panDragStartY = 0;
        var panDragBaseH = 0, panDragBaseV = 0;
        var H_PAN_MIN = -800, H_PAN_MAX = 800;
        var V_PAN_MIN = -1000, V_PAN_MAX = 1000;
        var PAN_STEP_H = 20;   // pixels per arrow keypress
        var PAN_STEP_V = 30;   // mV per arrow keypress
        var keysHeld = {};      // track held keys for smooth repeat
        var keyPanInterval = null;

        function trueVolts(v) {
            if (activeProbe === 'ch2') return Math.round(v * 6.0);
            if (activeProbe === 'ch3') return Math.round(v - 1650);
            return Math.round(v);
        }

        function changeProbe(val) {
            activeProbe = val;
            traceData.fill(0); recentTrace = [];
            var label = (val === 'ch1') ? 'PATH 1 (LOGIC)' : (val === 'ch2') ? 'PATH 2 (HIGH V)' : 'PATH 3 (AC)';
            document.getElementById('legTxt').textContent = label;
            updateTrigger(trigLvl);
            drawGraph();
        }

        var wsHost = 'ws://' + location.hostname + ':81/';
        document.getElementById('wsinfo').textContent = 'WS: ' + wsHost;
        var ws = new WebSocket(wsHost);
        ws.onopen = function() { document.getElementById('statusDot').classList.remove('off'); document.getElementById('statusTxt').textContent = 'LIVE'; };
        ws.onclose = ws.onerror = function() { document.getElementById('statusDot').classList.add('off'); document.getElementById('statusTxt').textContent = 'OFFLINE'; };

        ws.onmessage = function(e) {
            if (!running || (singleMode && singleDone)) return;
            var p = e.data.split(',');
            if (p.length !== 3) return;

            var cv = 0;
            if (activeProbe === 'ch1') cv = parseInt(p[0]);
            else if (activeProbe === 'ch2') cv = parseInt(p[1]);
            else if (activeProbe === 'ch3') cv = parseInt(p[2]);

            spsC++;
            var now = Date.now();
            if (now - spsT >= 1000) { document.getElementById('hdrSps').textContent = spsC; spsC = 0; spsT = now; }

            var crossed = (trigEdge === 'rise') ? (lastV < trigLvl && cv >= trigLvl) : (lastV > trigLvl && cv <= trigLvl);
            if (crossed) {
                if (lastCross !== null) {
                    var period = now - lastCross;
                    var freq = 1000 / period;
                    document.getElementById('hdrFreq').textContent = freq.toFixed(1) + ' Hz';
                    document.getElementById('hdrPeriod').textContent = period.toFixed(2) + ' ms';
                    document.getElementById('mFreq').textContent = freq.toFixed(1) + ' Hz';
                    document.getElementById('mPer').textContent = period.toFixed(2) + ' ms';
                    statsFreqArr.push(freq); if (statsFreqArr.length > 200) statsFreqArr.shift();
                }
                lastCross = now;
            }
            lastV = cv;
            recentTrace.push(cv); if (recentTrace.length > 500) recentTrace.shift();
            if (recentTrace.length >= 20 && spsC % 5 === 0) updateMeasurements();

            acqCount++; document.getElementById('hdrAcq').textContent = acqCount;

            if (dispMode === 'roll') {
                traceData.copyWithin(0, 1); traceData[bufSize - 1] = cv;
                drawGraph();
            } else {
                if (!triggered) { if (crossed) { triggered = true; ptIdx = 0; } }
                if (triggered) {
                    traceData[ptIdx] = cv; ptIdx++;
                    if (ptIdx >= ptsDraw) { triggered = false; if (singleMode) { singleDone = true; running = false; updateRunUI(); } }
                    drawGraph();
                }
            }
        };

        function updateMeasurements() {
            var cap = recentTrace, n = cap.length; if (n < 10) return;
            var mn = Infinity, mx = -Infinity, sum = 0, sumSq = 0, highs = 0;
            for (var i = 0; i < n; i++) {
                var v = cap[i];
                if (v < mn) mn = v; if (v > mx) mx = v;
                sum += v; sumSq += v * v;
                if (v >= trigLvl) highs++;
            }
            var avg = sum / n, rms = Math.sqrt(sumSq / n);
            document.getElementById('mVmax1').textContent = trueVolts(mx) + ' mV';
            document.getElementById('mVmin1').textContent = trueVolts(mn) + ' mV';
            document.getElementById('mVpp1').textContent = (trueVolts(mx) - trueVolts(mn)) + ' mV';
            document.getElementById('mVavg1').textContent = trueVolts(avg) + ' mV';
            document.getElementById('mVrms1').textContent = trueVolts(rms) + ' mV';
            document.getElementById('mDuty1').textContent = Math.round((highs / n) * 100) + ' %';
            document.getElementById('mVmax').textContent = trueVolts(mx) + ' mV';
            document.getElementById('mVmin').textContent = trueVolts(mn) + ' mV';
            document.getElementById('mVavg').textContent = trueVolts(avg) + ' mV';
            document.getElementById('hdrVpp').textContent = (trueVolts(mx) - trueVolts(mn)) + ' mV';
            statsVavgArr.push(avg); if (statsVavgArr.length > 200) statsVavgArr.shift();
            statsVppArr.push(mx - mn); if (statsVppArr.length > 200) statsVppArr.shift();
            updateStatsPanel();
        }

        function updateStatsPanel() {
            var na = statsVavgArr.length; if (na < 2) return;
            statsAcqCount++; document.getElementById('stAcq').textContent = statsAcqCount;
            var mean = statsVavgArr.reduce(function(a,b){return a+b;},0) / na;
            document.getElementById('stMean').textContent = trueVolts(mean) + ' mV';
            if (statsVppArr.length) {
                document.getElementById('stMinVpp').textContent = trueVolts(Math.min.apply(null, statsVppArr)) + ' mV';
                document.getElementById('stMaxVpp').textContent = trueVolts(Math.max.apply(null, statsVppArr)) + ' mV';
            }
        }
        function clearStats() { statsVavgArr=[]; statsVppArr=[]; statsFreqArr=[]; statsAcqCount=0; ['stAcq','stMean','stMinVpp','stMaxVpp'].forEach(function(id){document.getElementById(id).textContent='--';}); }

        function toggleRun() { running = !running; if (running){singleMode=false;singleDone=false;} updateRunUI(); }
        function updateRunUI() {
            var btn=document.getElementById('runBtn'), dot=document.getElementById('rbdot'), txt=document.getElementById('runTxt');
            if (running) {
                btn.classList.remove('stopped'); txt.textContent='RUNNING';
                dot.style.cssText='background:var(--acc);box-shadow:0 0 5px var(--acc);animation:blink 1.4s infinite;display:inline-block;width:7px;height:7px;border-radius:50%;margin-right:6px;';
            } else {
                btn.classList.add('stopped'); txt.textContent=singleMode?'SINGLE':'PAUSED';
                dot.style.cssText='background:#ff5040;box-shadow:0 0 5px #ff5040;animation:none;display:inline-block;width:7px;height:7px;border-radius:50%;margin-right:6px;';
            }
        }
        function doSingle() { singleMode=true; singleDone=false; running=true; triggered=false; ptIdx=0; changeMode('trigger'); updateRunUI(); }
        function changeMode(val) { dispMode=val; triggered=false; ptIdx=0; document.getElementById('modeRoll').classList.toggle('active',val==='roll'); document.getElementById('modeTrig').classList.toggle('active',val==='trigger'); drawGraph(); }
        function changeView(val) { viewMode=val; document.getElementById('viewTime').classList.toggle('active',val==='time'); document.getElementById('viewFft').classList.toggle('active',val==='fft'); drawGraph(); }
        function updateTrigger(val) { trigLvl=parseInt(val); document.getElementById('trigText').textContent=trueVolts(trigLvl)+'mV'; drawGraph(); }
        function setEdge(val) { trigEdge=val; document.getElementById('edgeRise').classList.toggle('active',val==='rise'); document.getElementById('edgeFall').classList.toggle('active',val==='fall'); }
        function updateTimebase(val) { timebase=parseInt(val); document.getElementById('timeText').textContent=val+'x'; ptsDraw=Math.floor(canvas.width/timebase); drawGraph(); }
        function setPersist(val) { persistAlpha=val; if(val===0) persistBuf=[]; document.getElementById('persOff').classList.toggle('active',val===0); document.getElementById('pers1').classList.toggle('active',val===0.92); document.getElementById('pers2').classList.toggle('active',val===0.97); drawGraph(); }
        function setCursorMode(mode) { cursorMode=mode; drawGraph(); }
        function switchRPanel(id,btn) { ['meas','cursor','stats'].forEach(function(tab){document.getElementById('rpanel-'+tab).style.display=(tab===id)?'block':'none';}); document.querySelectorAll('.rpanel-tab button').forEach(function(b){b.classList.remove('active');}); btn.classList.add('active'); }

        // ── PAN FUNCTIONS ──

        function clampHPos(v) { return Math.max(H_PAN_MIN, Math.min(H_PAN_MAX, v)); }
        function clampVPos(v) { return Math.max(V_PAN_MIN, Math.min(V_PAN_MAX, v)); }

        function applyHPan(newH) {
            hPos = clampHPos(Math.round(newH));
            document.getElementById('hposSlider').value = hPos;
            document.getElementById('hposText').textContent = hPos + 'px';
            document.getElementById('prHPos').textContent = hPos + ' px';
            document.getElementById('psH').textContent = hPos;
            updatePanIndicator();
            updatePanStatus();
        }

        function applyVPan(newV) {
            ofs = clampVPos(Math.round(newV));
            document.getElementById('ofsSlider').value = ofs;
            document.getElementById('ofsText').textContent = ofs;
            document.getElementById('prVPos').textContent = trueVolts(ofs) + ' mV';
            document.getElementById('psV').textContent = trueVolts(ofs);
            updatePanStatus();
        }

        function updateHPos(val) { applyHPan(parseInt(val)); drawGraph(); }
        function setOffset(val)  { applyVPan(parseInt(val)); drawGraph(); }

        function resetHPos() { applyHPan(0); drawGraph(); }
        function resetVPos() { applyVPan(0); drawGraph(); }
        function resetPan()  { applyHPan(0); applyVPan(0); drawGraph(); }

        function setGain(val) { gain=parseFloat(val); document.getElementById('gainText').textContent=gain.toFixed(1)+'x'; drawGraph(); }

        // Update the horizontal pan position indicator bar
        function updatePanIndicator() {
            var range = H_PAN_MAX - H_PAN_MIN;
            var norm = (hPos - H_PAN_MIN) / range; // 0..1
            var thumbW = Math.max(8, 100 / (range / 100)); // proportional thumb
            var thumbL = norm * (100 - thumbW);
            var thumb = document.getElementById('panThumb');
            thumb.style.left = thumbL.toFixed(1) + '%';
            thumb.style.width = thumbW.toFixed(1) + '%';
        }

        function updatePanStatus() {
            var ps = document.getElementById('panStatus');
            ps.classList.toggle('active', hPos !== 0 || ofs !== 0);
        }

        // Flash the arrow key indicator on canvas overlay
        function flashKey(dir) {
            var ids = { left:'pkLeft', right:'pkRight', up:'pkUp', down:'pkDown' };
            var el = document.getElementById(ids[dir]);
            if (!el) return;
            el.classList.add('lit');
            document.getElementById('panOverlay').classList.add('active');
            setTimeout(function(){ el.classList.remove('lit'); }, 150);
        }

        // ── DRAG TO PAN ──
        // Middle mouse button OR Alt+drag = pan
        // Shift+drag = pan (convenient on touchpad)
        // Right-click context menu blocked during pan

        canvas.addEventListener('mousedown', function(e) {
            // Pan on: middle button, Alt+left, or Shift+left
            var isPanDrag = (e.button === 1) || (e.button === 0 && (e.altKey || e.shiftKey));

            if (isPanDrag) {
                e.preventDefault();
                panDragging = true;
                panDragStartX = e.clientX;
                panDragStartY = e.clientY;
                panDragBaseH = hPos;
                panDragBaseV = ofs;
                canvas.classList.add('panning');
                return;
            }

            // Cursor dragging (existing)
            if (cursorMode === 'off') return;
            var r = canvas.getBoundingClientRect(), sx = canvas.width/r.width, sy = canvas.height/r.height;
            var cx = (e.clientX-r.left)*sx/canvas.width, cy = (e.clientY-r.top)*sy/canvas.height, thr = 0.02;
            if ((cursorMode==='x'||cursorMode==='xy')){if(Math.abs(cx-curX1)<thr){draggingCursor='x1';return;}if(Math.abs(cx-curX2)<thr){draggingCursor='x2';return;}}
            if ((cursorMode==='y'||cursorMode==='xy')){if(Math.abs(cy-curY1)<thr){draggingCursor='y1';return;}if(Math.abs(cy-curY2)<thr){draggingCursor='y2';return;}}
        });

        canvas.addEventListener('mousemove', function(e) {
            var r = canvas.getBoundingClientRect(), sx = canvas.width/r.width, sy = canvas.height/r.height;
            var px = (e.clientX-r.left)*sx, py = (e.clientY-r.top)*sy;
            mouseX = px; mouseY = py;
            var nx = px/canvas.width, ny = py/canvas.height;

            if (panDragging) {
                var dx = e.clientX - panDragStartX;
                var dy = e.clientY - panDragStartY;
                applyHPan(panDragBaseH + dx);
                applyVPan(panDragBaseV - dy * 3); // vertical drag: 3mV per pixel
                drawGraph();
                return;
            }

            // Cursor dragging
            if (draggingCursor) {
                if(draggingCursor==='x1') curX1=Math.max(0,Math.min(1,nx));
                if(draggingCursor==='x2') curX2=Math.max(0,Math.min(1,nx));
                if(draggingCursor==='y1') curY1=Math.max(0,Math.min(1,ny));
                if(draggingCursor==='y2') curY2=Math.max(0,Math.min(1,ny));
                updateCursorReadout();
            }

            // Cursor hover style: show grab when alt/shift held
            if (e.altKey || e.shiftKey) {
                canvas.classList.add('pan-ready');
            } else {
                canvas.classList.remove('pan-ready');
            }

            var tMs = Math.round((px/canvas.width)*ptsDraw*0.5);
            var vMv = trueVolts(Math.round((1-py/canvas.height)*3300));
            document.getElementById('cT').textContent = tMs;
            document.getElementById('cV').textContent = vMv;
            document.getElementById('cro').classList.add('vis');
            drawGraph(px, py);
        });

        canvas.addEventListener('mouseup', function(e) {
            if (panDragging) {
                panDragging = false;
                canvas.classList.remove('panning');
                return;
            }
            draggingCursor = null;
        });

        canvas.addEventListener('mouseleave', function() {
            panDragging = false;
            canvas.classList.remove('panning');
            canvas.classList.remove('pan-ready');
            draggingCursor = null;
            document.getElementById('cro').classList.remove('vis');
            mouseX = mouseY = undefined;
            drawGraph();
        });

        // Prevent right-click on canvas
        canvas.addEventListener('contextmenu', function(e){ e.preventDefault(); });

        // ── KEYBOARD PAN ──
        // Arrow keys: pan waveform
        // Shift+Arrow: fine pan (1/5 step)
        // Arrow key flash indicator

        document.addEventListener('keydown', function(e) {
            // Don't steal keys from inputs/selects
            if (document.activeElement && (document.activeElement.tagName === 'INPUT' || document.activeElement.tagName === 'SELECT')) return;

            var step, handled = true;
            switch(e.key) {
                case 'ArrowLeft':
                    step = e.shiftKey ? Math.round(PAN_STEP_H/5) : PAN_STEP_H;
                    applyHPan(hPos - step);
                    flashKey('left');
                    drawGraph();
                    break;
                case 'ArrowRight':
                    step = e.shiftKey ? Math.round(PAN_STEP_H/5) : PAN_STEP_H;
                    applyHPan(hPos + step);
                    flashKey('right');
                    drawGraph();
                    break;
                case 'ArrowUp':
                    step = e.shiftKey ? Math.round(PAN_STEP_V/5) : PAN_STEP_V;
                    applyVPan(ofs + step);
                    flashKey('up');
                    drawGraph();
                    break;
                case 'ArrowDown':
                    step = e.shiftKey ? Math.round(PAN_STEP_V/5) : PAN_STEP_V;
                    applyVPan(ofs - step);
                    flashKey('down');
                    drawGraph();
                    break;
                case 'Home':
                case '0':
                    resetPan();
                    break;
                default:
                    handled = false;
            }
            if (handled) e.preventDefault();
        });

        function resizeCanvas() {
            var wp = canvas.parentElement;
            var w = wp.clientWidth - 20, h = wp.clientHeight - 14;
            canvas.width = w; canvas.height = h;
            ptsDraw = Math.floor(canvas.width / timebase);
            updatePanIndicator();
            drawGraph();
        }
        window.addEventListener('resize', resizeCanvas);
        setTimeout(resizeCanvas, 80);

        function updateCursorReadout() {
            var W=canvas.width, H=canvas.height, ptsPerPx=1/timebase;
            var x1t=(curX1*W*ptsPerPx*0.5).toFixed(2), x2t=(curX2*W*ptsPerPx*0.5).toFixed(2), dx=Math.abs((curX2-curX1)*W*ptsPerPx*0.5);
            var y1v=trueVolts(Math.round((1-curY1)*3300)), y2v=trueVolts(Math.round((1-curY2)*3300)), dy=Math.abs(y1v-y2v);
            document.getElementById('cx1val').textContent=x1t+' µs'; document.getElementById('cx2val').textContent=x2t+' µs';
            document.getElementById('cy1val').textContent=y1v+' mV'; document.getElementById('cy2val').textContent=y2v+' mV';
            document.getElementById('cDX').textContent=dx.toFixed(2)+' µs'; document.getElementById('cDXF').textContent=dx>0?(1e6/dx).toFixed(0)+' Hz':'--';
            document.getElementById('cDY').textContent=dy+' mV';
        }

        function applyGainOfs(raw) { return Math.max(0, Math.min(3300, 1650 + (raw - 1650) * gain + ofs)); }

        function drawGrid() {
            var W=canvas.width, H=canvas.height;
            ctx.strokeStyle='rgba(0,191,255,0.05)'; ctx.lineWidth=0.5; ctx.beginPath();
            var hd=12, vd=8;
            for(var i=0;i<=hd;i++){var x=W/hd*i;ctx.moveTo(x,0);ctx.lineTo(x,H);}
            for(var j=0;j<=vd;j++){var y=H/vd*j;ctx.moveTo(0,y);ctx.lineTo(W,y);}
            ctx.stroke();
            ctx.fillStyle='rgba(0,191,255,0.08)';
            for(var di=1;di<hd;di++){for(var dj=1;dj<vd;dj++){ctx.fillRect(W/hd*di-1,H/vd*dj-1,2,2);}}
            ctx.strokeStyle='rgba(0,191,255,0.14)'; ctx.lineWidth=1; ctx.beginPath();
            ctx.moveTo(W/2,0);ctx.lineTo(W/2,H);ctx.moveTo(0,H/2);ctx.lineTo(W,H/2);ctx.stroke();

            // Draw a subtle pan-offset crosshair indicator at center when panned
            if (hPos !== 0 || ofs !== 0) {
                var cx0 = W/2 - hPos % (W/hd);
                ctx.strokeStyle='rgba(0,191,255,0.18)'; ctx.lineWidth=0.7;
                ctx.setLineDash([2,6]); ctx.beginPath();
                ctx.moveTo(W/2,0);ctx.lineTo(W/2,H);
                ctx.stroke(); ctx.setLineDash([]);
            }

            ctx.fillStyle='rgba(0,191,255,0.45)'; ctx.font='9px Share Tech Mono';
            var vls=['3.3V','2.5V','1.65V','0.8V','0V'];
            if(activeProbe==='ch2') vls=['19.8V','14.8V','9.9V','4.9V','0V'];
            else if(activeProbe==='ch3') vls=['+1.65V','+0.8V','0V','-0.8V','-1.65V'];
            var vps=[0,H*0.25,H*0.5,H*0.75,H];
            for(var k=0;k<vls.length;k++) ctx.fillText(vls[k],4,vps[k]+(k===0?10:k===4?-2:4));
            ctx.fillStyle='rgba(0,191,255,0.35)';
            for(var t=1;t<=4;t++){var xt=W/4*t;var tval=Math.round((xt/canvas.width)*ptsDraw*0.5);ctx.fillText(tval+'µ',xt-10,H-3);}
        }

        function drawTrigLine() {
            var H=canvas.height, W=canvas.width, ty=H-(trigLvl/3300)*H;
            ctx.save(); ctx.setLineDash([5,4]); ctx.strokeStyle='rgba(255,200,0,0.6)'; ctx.lineWidth=1; ctx.beginPath(); ctx.moveTo(0,ty); ctx.lineTo(W,ty); ctx.stroke(); ctx.setLineDash([]);
            ctx.fillStyle='rgba(255,200,0,0.8)'; ctx.beginPath(); ctx.moveTo(W-1,ty);ctx.lineTo(W-10,ty-5);ctx.lineTo(W-10,ty+5);ctx.closePath();ctx.fill();
            ctx.fillStyle='rgba(255,200,0,0.7)'; ctx.font='9px Share Tech Mono'; ctx.fillText('T '+trueVolts(trigLvl)+'mV',5,ty-3);
            ctx.restore();
        }

        function drawTrace(data, color, limit, mode, lineW) {
            if(limit===0) return;
            var W=canvas.width, H=canvas.height, si=(mode==='roll')?(bufSize-limit):0;
            ctx.save(); ctx.shadowColor=color; ctx.shadowBlur=5; ctx.strokeStyle=color; ctx.lineWidth=lineW||1.6; ctx.lineJoin='round'; ctx.beginPath();
            var first=true;
            for(var i=0;i<limit&&i*timebase<W+Math.abs(hPos)+100;i++){
                var y=H-(applyGainOfs(data[si+i])/3300)*H, x=i*timebase+hPos;
                if(first){ctx.moveTo(x,y);first=false;}else ctx.lineTo(x,y);
            }
            ctx.stroke(); ctx.restore();
        }

        function drawPersistTrace(data, color, limit, mode) {
            if(persistAlpha===0||limit===0) return;
            var frame=[], si=(mode==='roll')?(bufSize-limit):0;
            for(var i=0;i<limit;i++) frame.push(data[si+i]);
            persistBuf.push(frame); if(persistBuf.length>80) persistBuf.shift();
            var W=canvas.width, H=canvas.height;
            persistBuf.forEach(function(fr,fi){
                var alpha=0.08*Math.pow(persistAlpha,persistBuf.length-fi);
                ctx.save(); ctx.strokeStyle='rgba(0,191,255,'+alpha+')'; ctx.lineWidth=1; ctx.lineJoin='round'; ctx.beginPath();
                var first=true;
                for(var i=0;i<fr.length&&i*timebase<W+Math.abs(hPos)+100;i++){
                    var y=H-(applyGainOfs(fr[i])/3300)*H, x=i*timebase+hPos;
                    if(first){ctx.moveTo(x,y);first=false;}else ctx.lineTo(x,y);
                }
                ctx.stroke(); ctx.restore();
            });
        }

        function drawCursors() {
            var W=canvas.width, H=canvas.height; ctx.save(); ctx.setLineDash([5,3]); ctx.lineWidth=1;
            if(cursorMode==='x'||cursorMode==='xy'){
                var x1p=curX1*W; ctx.strokeStyle='rgba(255,255,0,0.7)'; ctx.beginPath();ctx.moveTo(x1p,0);ctx.lineTo(x1p,H);ctx.stroke();
                ctx.fillStyle='rgba(255,255,0,0.9)'; ctx.font='9px Share Tech Mono'; ctx.fillText('X1',x1p+3,12);
                var x2p=curX2*W; ctx.strokeStyle='rgba(255,136,255,0.7)'; ctx.beginPath();ctx.moveTo(x2p,0);ctx.lineTo(x2p,H);ctx.stroke();
                ctx.fillStyle='rgba(255,136,255,0.9)'; ctx.fillText('X2',x2p+3,12);
                ctx.fillStyle='rgba(255,255,200,0.04)'; ctx.fillRect(Math.min(x1p,x2p),0,Math.abs(x2p-x1p),H);
            }
            if(cursorMode==='y'||cursorMode==='xy'){
                var y1p=curY1*H; ctx.strokeStyle='rgba(255,255,0,0.7)'; ctx.beginPath();ctx.moveTo(0,y1p);ctx.lineTo(W,y1p);ctx.stroke();
                ctx.fillStyle='rgba(255,255,0,0.9)'; ctx.font='9px Share Tech Mono'; ctx.fillText('Y1 '+trueVolts(Math.round((1-curY1)*3300))+'mV',4,y1p-3);
                var y2p=curY2*H; ctx.strokeStyle='rgba(255,136,255,0.7)'; ctx.beginPath();ctx.moveTo(0,y2p);ctx.lineTo(W,y2p);ctx.stroke();
                ctx.fillStyle='rgba(255,136,255,0.9)'; ctx.fillText('Y2 '+trueVolts(Math.round((1-curY2)*3300))+'mV',4,y2p-3);
                ctx.fillStyle='rgba(255,255,200,0.04)'; ctx.fillRect(0,Math.min(y1p,y2p),W,Math.abs(y2p-y1p));
            }
            ctx.setLineDash([]); ctx.restore(); updateCursorReadout();
        }

        function drawCrosshair(cx, cy) {
            if(cx===undefined) return;
            var W=canvas.width, H=canvas.height; ctx.save(); ctx.setLineDash([2,3]); ctx.strokeStyle='rgba(255,255,255,0.15)'; ctx.lineWidth=0.7; ctx.beginPath();
            ctx.moveTo(cx,0);ctx.lineTo(cx,H);ctx.moveTo(0,cy);ctx.lineTo(W,cy);ctx.stroke();ctx.setLineDash([]);ctx.restore();
        }

        // Draw pan arrows on the canvas when panned
        function drawPanArrows() {
            if (hPos === 0 && ofs === 0) return;
            var W=canvas.width, H=canvas.height;
            ctx.save();
            ctx.font = 'bold 11px Share Tech Mono';
            ctx.fillStyle = 'rgba(0,191,255,0.5)';
            if (hPos > 0) { ctx.fillText('◀ ' + hPos + 'px', 8, H/2); }
            if (hPos < 0) { ctx.fillText('▶ ' + Math.abs(hPos) + 'px', W-60, H/2); }
            if (ofs > 0) { ctx.fillText('▼ ' + ofs, W/2 - 20, H-8); }
            if (ofs < 0) { ctx.fillText('▲ ' + Math.abs(ofs), W/2 - 20, 16); }
            ctx.restore();
        }

        function drawGraph(cx, cy) {
            var W=canvas.width, H=canvas.height;
            ctx.clearRect(0,0,W,H); ctx.fillStyle='#020e07'; ctx.fillRect(0,0,W,H);
            drawGrid(); drawTrigLine();
            var lim=(dispMode==='trigger')?ptIdx:ptsDraw;
            if(viewMode==='time'){
                if(persistAlpha>0) drawPersistTrace(traceData,'rgba(0,191,255,0.5)',lim,dispMode);
                drawTrace(traceData,'rgba(0,191,255,0.9)',lim,dispMode,1.8);
            }
            drawPanArrows();
            if(cursorMode!=='off') drawCursors();
            drawCrosshair(cx||mouseX, cy||mouseY);
        }

        // Initialize
        updatePanIndicator();
        updatePanStatus();
        drawGraph();
    </script>
</body>

</html>
)=====";
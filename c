<!DOCTYPE html>
<html>
<head>
    <title>Mleko Run - Telegram Edition</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0">
    <script src="https://telegram.org/js/games.js"></script>
    <style>
        body { margin: 0; overflow: hidden; background: #70c5ce; font-family: sans-serif; }
        canvas { display: block; margin: 0 auto; background: #eee; }
    </style>
</head>
<body>
    <canvas id="gameCanvas"></canvas>

<div id="deathScreen" style="display:none; position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); background: rgba(0,0,0,0.8); color: white; padding: 30px; border-radius: 15px; text-align: center; font-family: Arial; border: 4px solid #fff;">
    <h1 style="margin-top: 0;">IGRA GOTOVA</h1>
    <p style="font-size: 24px;">Tvoj skor: <span id="finalScore">0</span></p>
    <button onclick="resetGame()" style="padding: 15px 30px; font-size: 20px; font-weight: bold; background: #2e7d32; color: white; border: none; border-radius: 10px; cursor: pointer;">HELL YEAH</button>
</div>

<script>
const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");

canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

// --- 1. SLIKE ---
let birdImg = new Image();
birdImg.src = 'muk.png'; 

let pipeImg = new Image();
pipeImg.src = 'stub.png'; 

let bgImg = new Image();
bgImg.src = 'pozadina.png'; 

// --- 2. PARAMETRI IGRE ---
let bird = { 
    x: 60, 
    y: canvas.height/2, 
    w: 100,      
    h: 100, 
    gravity: 0.5, 
    lift: -9, 
    velocity: 0 
};

let pipes = [];
let frame = 0;
let score = 0;
let gameSpeed = 4; 
let pipeInterval = 100; // Početni razmak
let gameOver = false;

function drawBackground() {
    if (bgImg.complete && bgImg.width > 0) {
        ctx.drawImage(bgImg, 0, 0, canvas.width, canvas.height);
    } else {
        ctx.fillStyle = "#70c5ce"; 
        ctx.fillRect(0, 0, canvas.width, canvas.height);
    }
}

function drawBird() {
    ctx.drawImage(birdImg, bird.x, bird.y, bird.w, bird.h);
}

function createPipe() {
    // Rupa se smanjuje kako score raste (početna 280, minimalna 190)
    let currentGap = 280;
    if (score >= 10) currentGap = 240;
    if (score >= 50) currentGap = 190; // Ozbiljno uska rupa

    let minHeight = 100;
    let height = Math.floor(Math.random() * (canvas.height - currentGap - minHeight * 2)) + minHeight;
    pipes.push({ 
        x: canvas.width, 
        top: height, 
        bottom: canvas.height - height - currentGap, 
        passed: false,
        w: 120 
    });
}

function drawPipes() {
    pipes.forEach(p => {
        if (pipeImg.complete && pipeImg.width > 0) {
            ctx.drawImage(pipeImg, p.x, 0, p.w, p.top); 
            ctx.drawImage(pipeImg, p.x, canvas.height - p.bottom, p.w, p.bottom); 
        } else {
            ctx.fillStyle = "#2e7d32";
            ctx.fillRect(p.x, 0, p.w, p.top);
            ctx.fillRect(p.x, canvas.height - p.bottom, p.w, p.bottom);
        }
    });
}

function updatePipes() {
    if (score < 10) {
        gameSpeed = 4;
        pipeInterval = 100;
    } 
    else if (score >= 10 && score < 50) {
        gameSpeed = 6;
        pipeInterval = 80;
    } 
    else if (score >= 50) {
        gameSpeed = 9;      // Mnogo brže (bilo 7.5)
        pipeInterval = 55;  // Stubovi iskaču jedan za drugim (bilo 65)
    }

    if (frame % pipeInterval === 0) createPipe();
    
    // ... ostatak koda ostaje isti ...
    
    pipes.forEach(p => {
        p.x -= gameSpeed;

        if (!p.passed && bird.x > p.x + p.w) {
            score++;
            p.passed = true;
        }

        let margin = 12; 
        if (bird.x + bird.w - margin > p.x && bird.x + margin < p.x + p.w) {
            if (bird.y + margin < p.top || bird.y + bird.h - margin > canvas.height - p.bottom) {
                endGame();
            }
        }
    });

    pipes = pipes.filter(p => p.x > -p.w);
}

function updateBird() {
    bird.velocity += bird.gravity;
    bird.y += bird.velocity;
    if (bird.y + bird.h > canvas.height || bird.y < 0) endGame();
}

// IZMENJENA FUNKCIJA ZA KRAJ IGRE SA TELEGRAM TABELOM
function endGame() {
    gameOver = true;

    // Javljanje rezultata Telegramu za High Score tabelu u grupi
    if (typeof TelegramGameProxy !== 'undefined') {
        TelegramGameProxy.shareScore();
    }

    document.getElementById("finalScore").innerText = score;
    document.getElementById("deathScreen").style.display = "block";
}

function resetGame() {
    document.getElementById("deathScreen").style.display = "none";
    bird.y = canvas.height / 2;
    bird.velocity = 0;
    pipes = [];
    score = 0;
    frame = 0;
    gameSpeed = 4;
    pipeInterval = 100;
    gameOver = false;
    loop();
}

function loop() {
    if (gameOver) return; 
    
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    drawBackground();
    drawBird();
    updateBird();
    drawPipes();
    updatePipes();
    
    ctx.fillStyle = "white";
    ctx.strokeStyle = "black";
    ctx.lineWidth = 3;
    ctx.font = "bold 50px Arial";
    ctx.strokeText(score, canvas.width/2 - 20, 80);
    ctx.fillText(score, canvas.width/2 - 20, 80);
    
    frame++;
    requestAnimationFrame(loop);
}

window.addEventListener("mousedown", () => bird.velocity = bird.lift);
window.addEventListener("touchstart", (e) => {
    e.preventDefault();
    bird.velocity = bird.lift;
}, {passive: false});

loop();
</script>
</body>
</html>


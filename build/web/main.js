async function fetchStats() {
    try {
        const res = await fetch('/api/statistics');
        const data = await res.json();
        document.getElementById('processed').innerText = data.processed;
        document.getElementById('success_rate').innerText = data.success_rate.toFixed(2) + '%';
    } catch (err) {
        console.error('Error fetching stats:', err);
    }
}

async function fetchVideos() {
    try {
        const res = await fetch('/api/videos');
        const files = await res.json();
        const list = document.getElementById('videoList');
        list.innerHTML = '';
        files.forEach(f => {
            const li = document.createElement('li');
            li.textContent = f;
            list.appendChild(li);
        });
    } catch (err) {
        console.error('Error fetching videos:', err);
    }
}

function update() {
    fetchStats();
    fetchVideos();
}

// Run every 1 second
setInterval(update, 1000);

const express = require('express');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 8080;
const UPLOAD_DIR = path.join(__dirname, 'uploaded_videos');

// Serve static frontend files
app.use(express.static(path.join(__dirname, 'web')));

// API endpoint for statistics
app.get('/api/statistics', (req, res) => {
    try {
        const files = fs.readdirSync(UPLOAD_DIR).filter(f => f.endsWith('.mp4'));
        const processed = files.length;
        const successRate = processed > 0 ? 100 : 0;

        res.json({
            total_received: processed,
            total_processed: processed,
            total_dropped: 0,
            total_duplicates: 0,
            success_rate: successRate
        });
    } catch (err) {
        res.json({
            total_received: 0,
            total_processed: 0,
            total_dropped: 0,
            total_duplicates: 0,
            success_rate: 0
        });
    }
});

// API endpoint for videos
app.get('/api/videos', (req, res) => {
    try {
        const files = fs.readdirSync(UPLOAD_DIR)
            .filter(f => f.endsWith('.mp4'))
            .map(f => ({
                filename: f,
                file_path: `/uploaded_videos/${f}`,
                file_size: fs.statSync(path.join(UPLOAD_DIR, f)).size,
                producer_id: 1,       // placeholder
                upload_timestamp: Date.now() / 1000,
                is_duplicate: false
            }));
        res.json(files);
    } catch (err) {
        res.json([]);
    }
});

// Serve uploaded videos folder
app.use('/uploaded_videos', express.static(UPLOAD_DIR));

// Start server
app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
});

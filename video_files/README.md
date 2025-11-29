# Video Files Directory

Place your test video files here for each producer.

## Directory Structure

```
video_files/
├── producer_1/
│   ├── video1.mp4
│   └── video2.mp4
├── producer_2/
│   ├── video3.mp4
│   └── video4.mp4
└── producer_3/
    └── video5.mp4
```

## Supported Video Formats

- .mp4
- .avi
- .mov
- .mkv
- .flv
- .wmv
- .webm
- .m4v

## Automatic Test Videos

If you have FFmpeg installed, the `setup.sh` script will automatically generate small test videos for you.

Otherwise, manually add your own video files to each producer folder.

## Example: Add Videos Manually

```bash
cp /path/to/your/video1.mp4 video_files/producer_1/
cp /path/to/your/video2.mp4 video_files/producer_2/
cp /path/to/your/video3.mp4 video_files/producer_3/
```

Each producer will scan its assigned folder and upload all video files found.

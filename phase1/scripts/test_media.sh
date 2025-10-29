#!/usr/bin/env bash
set -euo pipefail

DATA_DIR="${1:-test_media}"
mkdir -p "$DATA_DIR"

FOLDER_ID="13IRWks6j3i9zMB3fqTGx_m7L3I_EDCg4"   # <-- Replace with your folder's ID

echo "Downloading all files from Google Drive folder ID: $FOLDER_ID"
echo "Saving to: $DATA_DIR"

# Install gdown if not available
if ! command -v gdown &> /dev/null; then
    echo "Installing gdown..."
    pip install --quiet gdown
fi

if [ "$(find "$DATA_DIR" -type f | wc -l)" -gt 0 ]; then
    echo "Files already present in $DATA_DIR — skipping download."
    exit 0
fi


# Download all contents of the folder recursively
gdown --folder "https://drive.google.com/drive/folders/$FOLDER_ID" -O "$DATA_DIR"

echo "Download complete!"






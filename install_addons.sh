#!/bin/bash

# Define the relative directory where you want to clone the repositories
RELATIVE_DIR="../../../addons/"

# List of GitHub repositories you want to clone
REPOS=("https://github.com/jeffcrouse/ofxJSON" "https://github.com/Daandelange/ofxImGui" "https://github.com/leadedge/ofxNDI")

# Loop through each repository and clone it into the relative directory
for repo in "${REPOS[@]}"
do
    # Extract the repository name from the URL
    repo_name=$(basename "$repo" .git)

    # Clone the repository into the relative directory
    git clone "$repo" "$RELATIVE_DIR/$repo_name"
done

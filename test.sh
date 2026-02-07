#!/bin/bash
set -eou pipefail

# Check if jq is available
if ! command -v jq >/dev/null 2>&1; then
    echo "Error: jq is required but not found. Please install jq." >&2
    exit 1
fi

# Parse arguments
SHUTDOWN=false
RELATIVE_PATH=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --shutdown)
            SHUTDOWN=true
            shift
            ;;
        *)
            if [ -z "$RELATIVE_PATH" ]; then
                RELATIVE_PATH="$1"
            else
                echo "Error: Multiple folder paths provided" >&2
                exit 1
            fi
            shift
            ;;
    esac
done

if [ -z "$RELATIVE_PATH" ]; then
    echo "Usage: $0 [--shutdown] <relative_folder_path>" >&2
    exit 1
fi

LANGUAGE_ID="hack"
SOURCE_FOLDER="$(pwd)/${RELATIVE_PATH}"
ID=0


# Check if folder exists
if [ ! -d "$SOURCE_FOLDER" ]; then
    echo "Error: Folder does not exist: $SOURCE_FOLDER" >&2
    exit 1
fi

log(){
    local message=$1
    printf "\n\033[33m[CLIENT]:\033[0m%s\n" "$message" >&2
}

echo "[INFO] Using source folder: $SOURCE_FOLDER" >&2

send_request(){
    local id=$1
    local method=$2
    local params=$3
    
    local body="{\"jsonrpc\":\"2.0\",\"id\":${id},\"method\":\"${method}\",\"params\":${params}}"
    local content_length=${#body}

    # Use actual CRLF for header, but keep body as literal string
    printf "Content-Length: %d\r\n\r\n%s" "$content_length" "$body"

    ID=$((ID+1))
}

send_notification(){
    local method=$1
    local params=$2
    
    local body="{\"jsonrpc\":\"2.0\",\"method\":\"${method}\",\"params\":${params}}"
    local content_length=${#body}

    # Use actual CRLF for header, but keep body as literal string
    printf "Content-Length: %d\r\n\r\n%s" "$content_length" "$body"
}


send_initialize(){
    local root_uri="file://${SOURCE_FOLDER}"
    local process_id=$$
    
    # Build initialize params with proper capabilities
    local params
    params=$(jq -n \
        --argjson processId "$process_id" \
        --arg rootUri "$root_uri" \
        '{
            "processId": $processId,
            "rootUri": $rootUri,
            "capabilities": {},
            "clientInfo": {
                "name": "test-client",
                "version": "1.0.0"
            }
        }')
    
    log "REQUEST: Initialize"
    send_request "$ID" "initialize" "$params"
}

send_did_open(){
    # Handle case where glob doesn't match (no files)
    shopt -s nullglob
    for file in "$SOURCE_FOLDER"/*; do
        # Skip if not a regular file
        [ -f "$file" ] || continue
        
        # Skip files ending with "2" before extension (e.g., Add2.asm)
        local basename
        basename=$(basename "$file")
        local name_no_ext="${basename%.*}"
        if [[ "$name_no_ext" =~ 2$ ]]; then
            continue
        fi
        
        local file_path="$file"
        local file_uri="file://${file_path}"
        
        # Build params JSON using jq for proper escaping of all control chars including CR
        local params
        params=$(jq -n \
            --arg uri "$file_uri" \
            --arg languageId "$LANGUAGE_ID" \
            --rawfile text "$file" \
            '{"textDocument":{"uri":$uri,"languageId":$languageId,"version":1,"text":$text}}')
        
        log "NOTIFICATION: textDocument/didOpen ${file}"
        send_notification "textDocument/didOpen" "$params" || true
    done
    shopt -u nullglob
}

send_did_change(){
    # Handle case where glob doesn't match (no files)
    shopt -s nullglob
    for file in "$SOURCE_FOLDER"/*; do
        # Skip if not a regular file
        [ -f "$file" ] || continue
        
        # Only process files ending with "2" before extension (e.g., Add2.asm)
        local basename
        basename=$(basename "$file")
        local name_no_ext="${basename%.*}"
        if [[ ! "$name_no_ext" =~ 2$ ]]; then
            continue
        fi
        
        # Remove "2" from filename for URI to match the file opened with didOpen
        local dir_path
        dir_path=$(dirname "$file")
        local name_without_2="${name_no_ext%2}"
        local extension="${basename##*.}"
        local original_basename="${name_without_2}.${extension}"
        local original_file_path="${dir_path}/${original_basename}"
        local file_uri="file://${original_file_path}"
        
        # Build params JSON for didChange - need textDocument with version and contentChanges
        local params
        params=$(jq -n \
            --arg uri "$file_uri" \
            --rawfile text "$file" \
            '{"textDocument":{"uri":$uri,"version":2},"contentChanges":[{"text":$text}]}')
        
        log "NOTIFICATION: textDocument/didChange ${file}"
        send_notification "textDocument/didChange" "$params" || true
    done
    shopt -u nullglob
}

# Get random position in a file (returns line and character)
get_random_position(){
    local file=$1
    local line_count
    line_count=$(wc -l < "$file" 2>/dev/null || echo "0")
    
    # If file is empty or has no lines, use position 0,0
    if [ "$line_count" -eq 0 ]; then
        echo "0 0"
        return
    fi
    
    # Generate random line (0-indexed for LSP, so 0 to line_count-1)
    local random_line
    random_line=$((RANDOM % line_count))
    
    # Get the line content to determine max character
    local line_content
    line_content=$(sed -n "$((random_line + 1))p" "$file" 2>/dev/null || echo "")
    local line_length=${#line_content}
    
    # Generate random character position (0-indexed, 0 to line_length)
    local random_char
    random_char=$((RANDOM % (line_length + 1)))
    
    echo "$random_line $random_char"
}

send_completion_request(){
    # Get list of opened files (non-2 files)
    shopt -s nullglob
    local files=()
    for file in "$SOURCE_FOLDER"/*; do
        [ -f "$file" ] || continue
        local basename
        basename=$(basename "$file")
        local name_no_ext="${basename%.*}"
        if [[ ! "$name_no_ext" =~ 2$ ]]; then
            files+=("$file")
        fi
    done
    shopt -u nullglob
    
    # Pick a random file
    if [ ${#files[@]} -eq 0 ]; then
        return
    fi
    
    local random_idx
    random_idx=$((RANDOM % ${#files[@]}))
    local selected_file="${files[$random_idx]}"
    local file_uri="file://${selected_file}"
    
    # Get random position
    local position
    position=$(get_random_position "$selected_file")
    local line
    line=$(echo "$position" | cut -d' ' -f1)
    local character
    character=$(echo "$position" | cut -d' ' -f2)
    
    # Build params JSON
    local params
    params=$(jq -n \
        --arg uri "$file_uri" \
        --argjson line "$line" \
        --argjson character "$character" \
        '{"textDocument":{"uri":$uri},"position":{"line":$line,"character":$character}}')
    
    log "REQUEST: textDocument/completion at line $line, char $character in $(basename "$selected_file")"
    send_request "$ID" "textDocument/completion" "$params" || true
}

send_hover_request(){
    # Get list of opened files (non-2 files)
    shopt -s nullglob
    local files=()
    for file in "$SOURCE_FOLDER"/*; do
        [ -f "$file" ] || continue
        local basename
        basename=$(basename "$file")
        local name_no_ext="${basename%.*}"
        if [[ ! "$name_no_ext" =~ 2$ ]]; then
            files+=("$file")
        fi
    done
    shopt -u nullglob
    
    # Pick a random file
    if [ ${#files[@]} -eq 0 ]; then
        return
    fi
    
    local random_idx
    random_idx=$((RANDOM % ${#files[@]}))
    local selected_file="${files[$random_idx]}"
    local file_uri="file://${selected_file}"
    
    # Get random position
    local position
    position=$(get_random_position "$selected_file")
    local line
    line=$(echo "$position" | cut -d' ' -f1)
    local character
    character=$(echo "$position" | cut -d' ' -f2)
    
    # Build params JSON
    local params
    params=$(jq -n \
        --arg uri "$file_uri" \
        --argjson line "$line" \
        --argjson character "$character" \
        '{"textDocument":{"uri":$uri},"position":{"line":$line,"character":$character}}')
    
    log "REQUEST: textDocument/hover at line $line, char $character in $(basename "$selected_file")"
    send_request "$ID" "textDocument/hover" "$params" || true
}



send_initialize

log "NOTIFICATION: Initialized"
send_notification "initialized" "{}"

send_did_open
send_did_change

# Send completion and hover requests at random positions
send_completion_request
send_hover_request

# Shutdown sequence (only if --shutdown flag is provided)
if [ "$SHUTDOWN" = true ]; then
    # Wait for responses to be processed
    sleep 0.2

    log "REQUEST: Shutdown"
    send_request "$ID" "shutdown" "{}"

    # Wait for shutdown response
    sleep 0.1

    log "NOTIFICATION: Exit"
    send_notification "exit" "{}"
else
    sleep 2
    log "Script finished (server will detect EOF and may shut down)"
    log "Use --shutdown flag for proper LSP shutdown sequence"
fi





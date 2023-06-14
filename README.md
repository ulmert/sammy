# Sammy
Control/wrapper for Electrosmith's **Daisy Patch** to run the **speech software SAM** (Software Automatic Mouth) for the Commodore C64 (https://github.com/vidarh/SAM)

## Usage
Use encoder:

### Word/sentence slots
- Set **trigger mode** (push / turn: "**+**" Always, "**-**" None, "**?**" Random) for each word/sentence.
- **Edit** word/sentence (push to edit -> turn to set character position -> push to edit character -> turn to set character -> push to exit -> turn/position to end of line to save "**s**")    
- Push "**r**" to select **random word**

### Speech profile
- Push / turn ("Sam", "Elf", "Robot", "Guy", "Lady", "ET")  

### Reset
- Push "**R**" to **point at first word/sentence slot** 


## Controls
| Control | Description | Comment |
| --- | --- | --- |
| Ctrl 1 | Playback speed | |
| Ctrl 2 | Volume | |
| Ctrl 3 | Filter | |
| Ctrl 4 | Fragment size (re-trigger) | |
| Gate In 1 | Trigger | Trigger current word/sentence |
| Gate In 2 | Re-trigger | Re-trigger from current sample position - fragment size |
| Gate Out | Active during playback| |
| Out 1 | Mono output | |

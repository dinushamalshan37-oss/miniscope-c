# GitHub Upload Checklist — miniscope-c

## Repository setup

1. Open https://github.com/new and create a **public** repository named `miniscope-c`.
2. Copy the description below into the GitHub **Description** field.
3. **Do not tick** “Add a README”, “Add .gitignore” or “Choose a license”, because this project already contains those files.
4. Unzip the project, open its folder in VS Code and open the terminal **inside that project folder**.
5. Replace `YOUR_GITHUB_USERNAME` with your actual GitHub username in the commands below.

```bash
git init
git add .
git commit -m "Initial commit: miniscope-c portfolio MVP"
git branch -M main
git remote add origin https://github.com/YOUR_GITHUB_USERNAME/miniscope-c.git
git push -u origin main
```

**Description:** Lightweight C Linux terminal monitor for CPU, memory and process usage with optional CSV logging.

**Topics:** c, linux, systems-programming, procfs, terminal, monitoring

Suggested GitHub visibility: Public (only after checking the folder for personal secrets).

## Local setup

**Prerequisites:** Linux/WSL2, GCC, make

```bash
make
./miniscope --once
```

## Verification status

Compiled successfully and Linux CLI smoke tests passed.

## Recommended GitHub polish

- Open the GitHub repository page → **About** (gear icon) → edit Description + Topics.
- Keep the included `README.md`, `LICENSE`, `.gitignore`, and GitHub Actions workflow.
- After running the project, add 2–3 authentic screenshots under `screenshots/` and link them in the README.
- Add a small release version (e.g. `v1.0.0`) after testing.
- Don't upload `.env` credentials, tokens, generated build folders or `data/` files containing private content.
- Be clear that this is an **educational MVP** and list its incomplete features.

## Helpful terminal commands

```bash
git status
git log --oneline -5
git add .
git commit -m "Improve features and docs"
git push
```

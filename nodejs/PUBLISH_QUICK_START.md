# Quick Start: Publish indexdb-lite

## One-time Setup

```bash
# Login to npm (do this once)
npm login

# Verify access
npm whoami
```

## Publish the Package

```bash
cd nodejs

# Build (optional - will run automatically)
npm run build

# Publish to npm
npm publish
```

## Verify Publication

```bash
# View on npm
npm view indexdb-lite

# Or visit: https://www.npmjs.com/package/indexdb-lite
```

## Installation (for users)

```bash
npm install indexdb-lite
```

## Update & Republish

```bash
cd nodejs

# Update version in package.json
npm version patch  # or minor/major

# Publish
npm publish

# Tag release in git (optional)
git tag v1.0.0
git push origin main --tags
```

---

**Full guide**: See `PUBLISHING.md` for detailed instructions and troubleshooting.

**Package name**: `indexdb-lite`  
**Registry**: https://registry.npmjs.org/  
**Public**: Unscoped, available to all npm users

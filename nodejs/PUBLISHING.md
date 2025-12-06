# Publishing @krk/indexdb-lite to npm

This guide explains how to publish the `@krk/indexdb-lite` package to the npm registry.

## Prerequisites

1. **npm Account**: Create an account at https://www.npmjs.com/
2. **@krk Scope Access**: Ensure your npm account has permission to publish to the `@krk` scope
   - If you own the scope, no additional setup is needed
   - If you're invited to the scope, you'll receive an invitation to accept

## Setup (One-time)

### 1. Login to npm

```bash
npm login
```

This will prompt you for:
- Username
- Password
- Email address

Your credentials will be stored in `~/.npmrc`.

### 2. Verify Your Access

```bash
npm whoami
```

This should display your npm username.

### 3. Verify Scope Access

```bash
npm access list scopes
```

You should see `@krk` listed if you have access.

## Publishing Steps

### 1. Prepare the Release

```bash
cd nodejs
```

### 2. Update Version (if needed)

Edit `package.json` and increment the version:

```json
{
  "version": "1.0.1"
}
```

Follow [Semantic Versioning](https://semver.org/):
- MAJOR.MINOR.PATCH
- Example: 1.0.0 → 1.0.1 (patch), 1.1.0 (minor), 2.0.0 (major)

### 3. Run Pre-publish Checks

```bash
npm run build
npm test
```

The `prepublishOnly` script in `package.json` will automatically run these before publishing, but it's good to verify first.

### 4. Publish to npm

```bash
npm publish --access public
```

The `--access public` flag ensures your scoped package is publicly visible on npm.

**Alternative**: If `.npmrc` has `access=public` configured, you can simply:

```bash
npm publish
```

### 5. Verify Publication

Check that your package is published:

```bash
npm view @krk/indexdb-lite
```

Or visit: https://www.npmjs.com/package/@krk/indexdb-lite

## Installation (for users)

Users can install your published package with:

```bash
npm install @krk/indexdb-lite
```

Or with yarn:

```bash
yarn add @krk/indexdb-lite
```

## Updating the Package

### 1. Make Changes

Edit source files, update tests, etc.

### 2. Bump Version

Update `package.json` version:

```bash
npm version patch    # Auto-increment patch version
npm version minor    # Auto-increment minor version
npm version major    # Auto-increment major version
```

Or manually edit the version.

### 3. Commit and Tag

```bash
git add package.json package-lock.json
git commit -m "Bump version to 1.0.1"
git tag v1.0.1
git push origin main --tags
```

### 4. Publish

```bash
npm publish --access public
```

## Troubleshooting

### "You do not have permission to publish to scope @krk"

**Solution**: 
- Ensure you're logged in: `npm whoami`
- Verify scope access: `npm access list scopes`
- If the scope is not listed, contact the scope owner to add you as a collaborator

### "This package name is already taken"

**Solution**:
- The package `@krk/indexdb-lite` already exists
- Either use a different name or ensure you have write access to this scope

### "npm ERR! 404 Not Found - PUT https://registry.npmjs.org/@krk/indexdb-lite"

**Solution**:
- The scope doesn't exist yet
- Create the scope on npm (if you own it) or contact the owner
- Ensure your `.npmrc` points to the correct registry

### "npm ERR! code E403 - Forbidden"

**Solution**:
- You don't have permission to publish under this scope
- Check your npm account access: `npm access list collaborators @krk`
- Request access from the scope owner

## Package Configuration Reference

The following files control the publishing behavior:

- **`package.json`**: Defines package metadata, dependencies, and `publishConfig`
- **`.npmignore`**: Excludes files from the published package
- **`.npmrc`**: Configures npm behavior (registry, access level)
- **`binding.gyp`**: Specifies build configuration for native modules
- **`index.d.ts`**: TypeScript type definitions

## Best Practices

1. **Update CHANGELOG**: Keep a changelog of version history
2. **Test Before Publishing**: Run `npm test` to ensure everything works
3. **Use Semantic Versioning**: Follow MAJOR.MINOR.PATCH convention
4. **Tag Releases**: Use git tags to mark each release
5. **Document Changes**: Update README.md with new features/fixes
6. **Review Dependencies**: Keep dependencies up to date and audit for security
7. **Access Control**: Only publish from trusted machines with verified credentials

## CI/CD Integration (Optional)

You can automate publishing using GitHub Actions:

```yaml
name: Publish to npm

on:
  push:
    tags:
      - 'v*'

jobs:
  publish:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: actions/setup-node@v3
        with:
          node-version: '16'
          registry-url: 'https://registry.npmjs.org'
      
      - run: cd nodejs && npm install
      - run: cd nodejs && npm run build
      - run: cd nodejs && npm test
      - run: cd nodejs && npm publish --access public
        env:
          NODE_AUTH_TOKEN: ${{ secrets.NPM_TOKEN }}
```

## Support

For more information:
- [npm Scopes Documentation](https://docs.npmjs.com/about-scopes)
- [npm Publishing Guide](https://docs.npmjs.com/packages-and-modules/contributing-packages-to-the-registry)
- [Semantic Versioning](https://semver.org/)

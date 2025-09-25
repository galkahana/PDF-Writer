# PDF-Writer Claude Agents

This directory contains specialized Claude agents for PDF-Writer development and community support.

## Available Agents

### example-generator
**Purpose**: Creates example test files for community requests
**Usage**:
```bash
# For GitHub issues
/agent example-generator https://github.com/galkahana/PDF-Writer/issues/XXX

# For email requests
/agent example-generator "paste email content here"
```

**What it does**:
- Analyzes PDF functionality requests
- Assesses scope and feasibility
- Generates working example test files
- Creates branches and draft PRs
- Handles form operations (creation, filling, flattening, querying)

## Workflow Integration

### 1. Community Request Handling
When you receive requests via:
- GitHub issues asking for examples
- Email requests for PDF functionality demos
- Community forum questions about implementation

Simply launch the example-generator agent with the request details.

### 2. Agent Output
The agent will provide:
- **Assessment**: Feasibility analysis and scope estimation
- **Implementation Plan**: Step-by-step development approach
- **Working Code**: Complete test files following project conventions
- **Branch/PR**: Ready-to-review draft PR with example

### 3. Quality Assurance
All agent-generated examples:
- Follow PDF-Writer coding conventions (Hungarian notation, C99 compatibility)
- Include proper error handling (do-while-false pattern)
- Build successfully with cmake
- Pass testing with ctest
- Handle conditional compilation for optional dependencies
- Follow patterns established in PR #313

## Request Types Well-Suited for Automation

### ✅ Good Candidates
- PDF outlines/bookmarks creation
- Form creation, filling, flattening
- Image embedding (PNG, JPEG, TIFF)
- Basic annotation examples
- Password/encryption demonstrations
- Text rendering and font handling
- Color space examples

### ⚠️ May Need Review
- Complex multi-step workflows
- Platform-specific implementations
- Performance-critical examples
- Examples requiring new library features

### ❌ Not Suitable
- Major API changes or new features
- Complex multi-file examples
- Requests requiring unsupported external dependencies

## Agent Development Notes

Agents follow the development patterns discovered in active PDF-Writer development:
- Make common cases simple with convenient defaults
- Support both basic and advanced usage scenarios
- Extract reusable components into helper classes
- Create self-contained, educational examples
- Test incrementally and handle edge cases

For agent modifications or new agents, see `/Users/galk/Documents/projects/PDF-Writer/CLAUDE.md` for project-specific development guidelines.
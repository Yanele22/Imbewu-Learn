# Imbewu Learn - Complete Setup Guide

## Table of Contents
1. [Overview](#overview)
2. [Frontend Setup](#frontend-setup)
3. [Backend Setup](#backend-setup)
4. [Microsoft Foundry Integration](#microsoft-foundry-integration)
5. [Deployment](#deployment)
6. [Testing](#testing)

---

## Overview

Imbewu Learn is a context-aware AI platform that delivers culturally respectful learning experiences. The system:

1. **Collects context** - Asks learners about their cultural background, location, language, and goals
2. **Classifies requests** - Understands what knowledge is being sought
3. **Retrieves knowledge** - Searches community-contributed knowledge
4. **Personalizes responses** - Presents learning tailored to the learner's context

### Key Features
- Multi-turn conversation with context collection
- Culturally respectful knowledge delivery
- Secure API (keys never exposed to frontend)
- Integrates with Microsoft Foundry for AI
- GitHub Pages hosting for frontend
- Flexible backend deployment options

---

## Frontend Setup

### Option 1: Deploy to GitHub Pages (Recommended)

```bash
# 1. In your Imbewu-Learn repository root
git add imbewu-learn-v2.html learning-together.jpg
git commit -m "Add Ask Imbewu feature with context-aware AI"
git push origin master

# 2. In GitHub repository settings:
#    - Go to Settings → Pages
#    - Source: Branch: master
#    - Save
#
# 3. Your site is available at:
#    https://yanele22.github.io/Imbewu-Learn/imbewu-learn-v2.html

# 4. To make it the default index:
cp imbewu-learn-v2.html index.html
git add index.html
git commit -m "Set index.html for GitHub Pages"
git push
```

Your frontend will be live at: `https://yanele22.github.io/Imbewu-Learn/`

### Option 2: Local Testing

```bash
# Serve locally for testing
python3 -m http.server 8000

# Open http://localhost:8000/imbewu-learn-v2.html
```

---

## Backend Setup

The backend is optional for the demo, but required for production with Microsoft Foundry integration.

### Prerequisites
- Node.js 14+ or Python 3.8+
- npm (for Node.js)

### Node.js Backend Setup

```bash
# 1. Install dependencies
npm install

# 2. Create .env file
cp .env.example .env
# Edit .env with your Azure credentials (see below)

# 3. Start the server
npm start

# Server runs at http://localhost:3000
# API endpoint: POST http://localhost:3000/api/imbewu/chat

# For development with auto-reload:
npm run dev
```

### Python Backend Setup (Alternative)

```bash
# 1. Create virtual environment
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# 2. Install dependencies
pip install -r requirements.txt

# 3. Create .env file
cp .env.example .env
# Edit .env with your Azure credentials

# 4. Start the server
python server.py

# Server runs at http://localhost:5000
```

### Test the Backend

```bash
# Health check
curl http://localhost:3000/health

# Send a message
curl -X POST http://localhost:3000/api/imbewu/chat \
  -H "Content-Type: application/json" \
  -d '{
    "message": "How do I make umgombodi?",
    "sessionId": "test-user-123",
    "conversationHistory": []
  }'
```

Expected response:
```json
{
  "reply": "🌱 Imbewu AI: Thank you. Which culture or community does your knowledge come from?",
  "context": {
    "question": "How do I make umgombodi?"
  },
  "stage": "getting_culture",
  "sessionId": "test-user-123"
}
```

---

## Microsoft Foundry Integration

### Step 1: Create Azure Account

1. Go to [Azure Portal](https://portal.azure.com)
2. Sign up or sign in
3. Create a resource group: `Imbewu-Learn`

### Step 2: Create Azure OpenAI Resource

```bash
# Using Azure CLI
az login
az group create --name Imbewu-Learn --location eastus

az cognitiveservices account create \
  --name imbewu-foundry \
  --resource-group Imbewu-Learn \
  --kind OpenAI \
  --sku S0 \
  --location eastus
```

Or use Azure Portal:
1. Search "Azure OpenAI"
2. Create new resource
3. Resource group: `Imbewu-Learn`
4. Name: `imbewu-foundry`
5. Location: East US
6. Pricing tier: Standard S0

### Step 3: Deploy a Model

```bash
# Get your Azure OpenAI resource keys
RESOURCE_KEY=$(az cognitiveservices account keys list \
  --name imbewu-foundry \
  --resource-group Imbewu-Learn \
  --query key1 -o tsv)

echo "API Key: $RESOURCE_KEY"
```

In Azure Portal:
1. Go to your resource → Keys and Endpoint
2. Copy Key 1 and Endpoint
3. Go to Model deployments → Create
4. Model: gpt-4 or gpt-4-turbo
5. Deployment name: `imbewu-agent`

### Step 4: Create Azure AI Search (Optional - for knowledge base)

```bash
az search service create \
  --name imbewu-search \
  --resource-group Imbewu-Learn \
  --sku standard
```

Or use Azure Portal:
1. Search "Azure AI Search"
2. Create new service
3. Name: `imbewu-search`
4. Pricing tier: Standard

### Step 5: Update .env File

```bash
# Get your Foundry API key
export AZURE_FOUNDRY_KEY="your-key-from-azure-portal"
export AZURE_FOUNDRY_ENDPOINT="https://imbewu-foundry.openai.azure.com/"
export AZURE_FOUNDRY_DEPLOYMENT="imbewu-agent"

# Get your Search API key
export AZURE_SEARCH_KEY="your-search-key"
export AZURE_SEARCH_ENDPOINT="https://imbewu-search.search.windows.net/"
```

Create `.env` file:
```bash
cp .env.example .env

# Edit .env and paste your keys
nano .env  # or use your editor
```

### Step 6: Update Backend

The `server.js` currently uses a demo implementation. For production:

```javascript
// Replace the generateAimbewuResponse function with:
async function generateAimbewuResponse(message, history, context) {
  const response = await axios.post(
    `${process.env.AZURE_FOUNDRY_ENDPOINT}openai/deployments/${process.env.AZURE_FOUNDRY_DEPLOYMENT}/chat/completions`,
    {
      messages: [
        { role: 'system', content: IMBEWU_SYSTEM_PROMPT },
        ...history,
        { role: 'user', content: message }
      ],
      temperature: 0.7,
      max_tokens: 2048
    },
    {
      headers: {
        'api-key': process.env.AZURE_FOUNDRY_KEY,
        'api-version': process.env.AZURE_API_VERSION
      }
    }
  );

  const reply = response.data.choices[0].message.content;
  
  return {
    reply: `🌱 Imbewu AI: ${reply}`,
    context: context,
    stage: 'complete'
  };
}
```

---

## Deployment

### Option 1: Deploy Backend to Heroku

```bash
# Install Heroku CLI
# https://devcenter.heroku.com/articles/heroku-cli

# Login
heroku login

# Create app
heroku create imbewu-learn-api

# Set environment variables
heroku config:set AZURE_FOUNDRY_KEY="your-key" -a imbewu-learn-api
heroku config:set AZURE_FOUNDRY_ENDPOINT="your-endpoint" -a imbewu-learn-api
heroku config:set FRONTEND_URL="https://yanele22.github.io/Imbewu-Learn" -a imbewu-learn-api

# Deploy
git push heroku main

# View logs
heroku logs --tail -a imbewu-learn-api

# Your API is at: https://imbewu-learn-api.herokuapp.com/api/imbewu/chat
```

### Option 2: Deploy to Azure App Service

```bash
# Install Azure CLI
# https://learn.microsoft.com/en-us/cli/azure/install-azure-cli

# Login
az login

# Create App Service Plan
az appservice plan create \
  --name imbewu-plan \
  --resource-group Imbewu-Learn \
  --sku B1 --is-linux

# Create web app
az webapp create \
  --resource-group Imbewu-Learn \
  --plan imbewu-plan \
  --name imbewu-learn-api \
  --runtime "NODE|18-lts"

# Set environment variables
az webapp config appsettings set \
  --resource-group Imbewu-Learn \
  --name imbewu-learn-api \
  --settings \
    AZURE_FOUNDRY_KEY="your-key" \
    AZURE_FOUNDRY_ENDPOINT="your-endpoint" \
    FRONTEND_URL="https://yanele22.github.io/Imbewu-Learn"

# Deploy from git
az webapp deployment source config-zip \
  --resource-group Imbewu-Learn \
  --name imbewu-learn-api \
  --src api.zip
```

### Option 3: Deploy to Railway.app

```bash
# Install Railway CLI
npm i -g @railway/cli

# Login
railway login

# Initialize project
railway init

# Deploy
railway up

# Set environment variables in Railway dashboard
# Your API will be available at the provided Railway URL
```

### Update Frontend with Backend URL

In `imbewu-learn-v2.html`, update the `sendMessage()` function:

```javascript
async function sendMessage() {
  const input = document.getElementById('userInput');
  const text = input.value.trim();
  
  if (!text) return;
  
  addMessage(text, true);
  input.value = '';
  
  // Call backend API instead of local processing
  const backendURL = 'https://imbewu-learn-api.herokuapp.com/api/imbewu/chat';
  
  try {
    const response = await fetch(backendURL, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        message: text,
        sessionId: 'user-' + Date.now(),
        conversationHistory: conversationState.history || []
      })
    });
    
    const data = await response.json();
    addMessage(data.reply, false);
    Object.assign(conversationState.context, data.context);
    conversationState.stage = data.stage;
    updateContext();
  } catch (error) {
    addMessage('❌ Failed to connect to Imbewu. Please try again.', false);
    console.error('Backend error:', error);
  }
}
```

---

## Testing

### 1. Test Frontend Locally

```bash
python3 -m http.server 8000
# Visit http://localhost:8000/imbewu-learn-v2.html
# Try asking a question in the "Ask Imbewu" section
```

### 2. Test Backend API

```bash
# Terminal 1: Start backend
npm start

# Terminal 2: Test with curl
curl -X POST http://localhost:3000/api/imbewu/chat \
  -H "Content-Type: application/json" \
  -d '{
    "message": "How do I make umgombodi?",
    "sessionId": "test-123"
  }'
```

### 3. Test End-to-End

```bash
# Open http://localhost:8000/imbewu-learn-v2.html
# Type a question in the chat
# Watch the conversation flow collect context
```

### 4. Test with Production Backend

```bash
# Update the URL in frontend to use deployed backend
# Test from https://yanele22.github.io/Imbewu-Learn/
# Verify conversation flows correctly
```

---

## Troubleshooting

### "CORS Error"
- Ensure backend URL is in CORS whitelist
- Add `FRONTEND_URL` to `.env`
- Check backend is running

### "Azure API Key Invalid"
- Verify `.env` has correct `AZURE_FOUNDRY_KEY`
- Check key is from the correct resource
- Ensure key has not expired

### "Backend not responding"
- Check backend is running: `curl http://localhost:3000/health`
- Check firewall allows the port
- Check .env file exists with correct variables

### "Knowledge not found"
- Populate the knowledge base using your knowledge API
- Ensure Azure AI Search index is created
- Verify search connection string in .env

---

## Next Steps

1. **Set up Microsoft Foundry** with your Azure account
2. **Configure Azure AI Search** for knowledge base retrieval
3. **Deploy backend** to Heroku or Azure
4. **Populate knowledge base** with community knowledge
5. **Test end-to-end** conversation flow
6. **Go live** with GitHub Pages + backend API

---

## Resources

- [Azure OpenAI Documentation](https://learn.microsoft.com/en-us/azure/ai-services/openai/overview)
- [Microsoft Foundry Docs](https://learn.microsoft.com/en-us/azure/ai-services/)
- [Azure AI Search](https://learn.microsoft.com/en-us/azure/search/)
- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [Heroku Deployment](https://devcenter.heroku.com/)

---

## Support

For issues or questions:
1. Check the Architecture documentation: `ARCHITECTURE.md`
2. Review backend logs: `heroku logs --tail`
3. Test the API health endpoint: `/health`
4. Check browser console for frontend errors

Good luck with Imbewu Learn! 🌱

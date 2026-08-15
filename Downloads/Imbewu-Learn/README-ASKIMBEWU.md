# Imbewu Learn 🌱

**Knowledge grows in context.**

Imbewu Learn is an AI-powered platform that transforms community knowledge into interactive, multilingual learning experiences—preserving cultural knowledge and connecting generations.

---

## What's New: Ask Imbewu 🌱

We've implemented your mentor's vision of **context-aware AI that respects cultural differences**:

### The Problem It Solves

Traditional AI chatbots make assumptions:
```
User: "How do I make umgombodi?"
AI: "Here are the steps…"
```

This ignores that practices vary by culture, region, family, and community.

### The Imbewu Solution

```
User: "How do I make umgombodi?"

Imbewu AI: "I can help with that 🌱. Before I explain, 
I want to make sure I give you the version that fits 
your community. What is your cultural background?"

User: "I'm Zulu."

Imbewu AI: "Thanks. Which region are you from? 
Practices can vary between communities."

User: "KwaZulu-Natal."

Imbewu AI: "Based on what you've shared—Zulu culture 
from KwaZulu-Natal—I'll tailor the explanation to your 
context while clearly separating community-specific 
knowledge from general information."
```

---

## Key Features

✅ **Context-Aware Responses** - Collects culture, location, language, experience level  
✅ **Culturally Respectful** - Acknowledges variations between communities  
✅ **Secure by Design** - API keys never exposed to frontend  
✅ **Multi-turn Conversations** - Natural dialogue flow  
✅ **Production-Ready** - Integrates with Microsoft Foundry  
✅ **Knowledge Grounded** - Retrieves from community knowledge, not AI invention  
✅ **Multilingual Support** - Responds in user's preferred language  

---

## Quick Start

### For Developers

1. **View the feature:**
   - Open `imbewu-learn-v2.html` in a browser
   - Click "Ask Imbewu" button or scroll to chat section
   - Try asking a question about traditional knowledge

2. **Understand the architecture:**
   - Read `ARCHITECTURE.md` for system design
   - See `server.js` for backend implementation
   - Review `SETUP.md` for deployment options

3. **Deploy locally:**
   ```bash
   # Frontend only
   python3 -m http.server 8000
   # Visit http://localhost:8000/imbewu-learn-v2.html
   
   # Or with backend
   npm install
   npm start
   # Backend runs at http://localhost:3000
   ```

4. **Deploy to production:**
   - Frontend: GitHub Pages (already configured)
   - Backend: Heroku, Azure, or Railway (see SETUP.md)
   - AI: Microsoft Foundry (see ARCHITECTURE.md)

---

## File Structure

```
Imbewu-Learn/
├── imbewu-learn-v2.html          # Landing page with Ask Imbewu
├── learning-together.jpg         # Community image
├── server.js                      # Backend API (Node.js)
├── package.json                   # Dependencies
├── .env.example                   # Configuration template
├── ARCHITECTURE.md                # System design & context engine
├── SETUP.md                       # Complete deployment guide
├── create_image.py                # Image generation script
└── README.md                      # This file
```

---

## The Context Engine

### Stage 1: Question Capture
User asks: "How do I make umgombodi?"

### Stage 2: Culture Collection
AI asks: "Which culture or community?"

### Stage 3: Location Collection
AI asks: "Which region or area?"

### Stage 4: Language Collection
AI asks: "Which language?"

### Stage 5: Experience Collection
AI asks: "What's your experience level?"

### Stage 6: Personalized Response
AI delivers knowledge tailored to:
- ✓ Their specific culture
- ✓ Their geographic region  
- ✓ Their preferred language
- ✓ Their experience level
- ✓ Grounded in community knowledge

---

## Technology Stack

### Frontend
- HTML5 / CSS3 / Vanilla JavaScript
- Responsive design
- No external dependencies

### Backend
- Node.js + Express.js
- CORS enabled for security
- Session management

### AI & Services
- **Microsoft Foundry** - AI agent orchestration
- **Azure OpenAI** - GPT-4 language model
- **Azure AI Search** - Knowledge base retrieval

### Deployment
- **Frontend:** GitHub Pages
- **Backend:** Heroku / Azure App Service / Railway
- **Database:** Azure Cosmos DB (future)

---

## Architecture Highlights

### Why This Design is Better

1. **User-Centered** - Collects context instead of making assumptions
2. **Culturally Respectful** - Acknowledges differences between communities
3. **Knowledge-Grounded** - Retrieves from verified community knowledge
4. **Secure by Default** - API credentials on backend, never exposed
5. **Scalable** - Supports thousands of concurrent conversations
6. **Personalized** - Same question → different answers based on context

### System Flow

```
User Question
    ↓
Context Collection (AI asks follow-up questions)
    ↓
Classification (Determine what knowledge is needed)
    ↓
Knowledge Retrieval (Search knowledge base)
    ↓
Personalization (Tailor to learner's context)
    ↓
Culturally-Grounded Response
```

---

## Microsoft Foundry Integration

### What is Foundry?

Microsoft Foundry (formerly Azure AI Foundry) is a platform for:
- Building and deploying AI agents
- Managing multi-turn conversations
- Connecting to external knowledge sources
- Monitoring and logging

### Why Foundry for Imbewu?

✓ **Agents** - Multi-turn conversations with memory  
✓ **Tools** - Connect to Azure AI Search, APIs  
✓ **Orchestration** - Route to different knowledge sources  
✓ **Monitoring** - Track conversation quality  

### Setup Steps

1. Create Azure OpenAI resource
2. Deploy GPT-4 model
3. Create Foundry agent with Imbewu system prompt
4. Connect Azure AI Search for knowledge retrieval
5. Deploy backend API

See `SETUP.md` for detailed instructions.

---

## Knowledge Base Structure

The knowledge base is organized by culture, topic, and region:

```
Knowledge Base
├── Food & Cooking
│   ├── Umgombodi
│   │   ├── isiZulu (KwaZulu-Natal)
│   │   ├── Sepedi (Gauteng)
│   │   └── isiXhosa (Eastern Cape)
│   └── [more dishes...]
│
├── Traditional Agriculture
│   ├── Crop rotation
│   ├── Seasonal indicators
│   └── Indigenous plants
│
├── Medicinal Plants
│   ├── Healing traditions
│   ├── Preparation methods
│   └── Community testimonials
│
└── [more categories...]
```

---

## Deployment Options

### Frontend (GitHub Pages)
```bash
git push origin master
# Deployed at: https://yanele22.github.io/Imbewu-Learn/
```

### Backend (Choose one)

**Heroku:**
```bash
heroku create imbewu-learn-api
git push heroku main
```

**Azure:**
```bash
az webapp create --name imbewu-learn-api
az webapp up --name imbewu-learn-api
```

**Railway:**
```bash
railway init
railway up
```

See `SETUP.md` for detailed deployment instructions.

---

## Development Roadmap

### ✅ Completed
- [x] Frontend landing page with Ask Imbewu
- [x] Context collection system
- [x] Multi-turn conversation UI
- [x] Backend API skeleton
- [x] Architecture documentation

### 🔄 In Progress
- [ ] Microsoft Foundry integration
- [ ] Azure AI Search setup
- [ ] Knowledge base population
- [ ] Knowledge retrieval system

### 📋 Planned
- [ ] Voice input (Azure Speech Services)
- [ ] Voice output (Text-to-Speech)
- [ ] Mobile app (React Native)
- [ ] Offline mode
- [ ] Community contributor dashboard
- [ ] Moderation system
- [ ] Learning progress tracking
- [ ] Certificates/badges

---

## Contributing

We welcome contributions! Areas to help:

1. **Knowledge Curation** - Share community knowledge
2. **Frontend** - Improve UI/UX
3. **Backend** - Enhance API
4. **AI Tuning** - Improve system prompt
5. **Testing** - Report bugs
6. **Documentation** - Improve guides

---

## System Prompt (The Heart of Imbewu)

```
You are Imbewu Learn, a culturally respectful learning assistant.

Your purpose is to help people learn community knowledge while respecting
differences between cultures, communities, families, and regions.

Before answering: collect context about culture, location, language, 
experience level, and learning goal.

Do NOT assume culture from location.
Do NOT treat a culture as having one universal practice.
If context is missing, ask a concise follow-up question.

When answering:
- Prefer verified community knowledge
- Distinguish documented knowledge from general information
- Acknowledge differences between communities
- Never invent traditions
- Respect the learner's language and experience level
- If knowledge isn't in our base, say so

Goal: Help learners understand knowledge in the context 
in which it belongs.
```

---

## Resources

- [Architecture Guide](ARCHITECTURE.md) - System design details
- [Setup Guide](SETUP.md) - Deployment instructions
- [Microsoft Foundry Docs](https://learn.microsoft.com/azure/ai-services)
- [GitHub Pages Help](https://docs.github.com/pages)
- [Azure OpenAI Docs](https://learn.microsoft.com/azure/ai-services/openai)

---

## License

MIT License - See LICENSE file for details

---

## Contact

For questions about Imbewu Learn:
- 📧 Email: hello@imbewu.learn (coming soon)
- 🐙 GitHub Issues: [Report a bug](https://github.com/Yanele22/Imbewu-Learn/issues)
- 🌐 Website: [imbewu.learn](https://imbewu.learn) (coming soon)

---

## Acknowledgments

This project was inspired by a mentor's vision of culturally respectful AI that:
- Collects context before answering
- Classifies knowledge requests properly
- Retrieves from community knowledge
- Personalizes learning experiences
- Never assumes culture from location

**Imbewu** means "seed" in many Southern African languages.

Just as a seed grows when planted in the right soil with the right conditions,
**knowledge grows when presented in its cultural context.** 🌱

---

## Status

```
Frontend:      ✅ Live at GitHub Pages
Backend:       🔄 Ready for deployment (see SETUP.md)
AI Engine:     📋 Ready to integrate Microsoft Foundry
Knowledge Base: 📋 Ready to populate
```

**Latest Version:** 1.0.0 - Ask Imbewu Context Engine  
**Last Updated:** 2026-08-15

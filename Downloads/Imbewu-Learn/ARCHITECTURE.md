# Imbewu Learn - Context-Aware AI Architecture

## Vision

Imbewu Learn uses a context-aware AI system (the "Imbewu Context Engine") to deliver culturally respectful, personalized learning experiences. The system collects learner context before providing answers, ensuring knowledge is presented in the right cultural, geographic, and linguistic frame.

## Core Principle

**Knowledge changes with context.**

Don't assume someone's culture from location. Don't treat a culture as having one universal practice. Instead:
1. **Collect context** - Ask the learner about their background
2. **Classify the request** - Understand what knowledge is being sought
3. **Retrieve relevant knowledge** - Search community-contributed knowledge
4. **Personalize the response** - Present learning tailored to their context

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    IMBEWU LEARN UI                          │
│  (GitHub Pages + HTML/CSS/JavaScript)                       │
│                                                             │
│  - Landing page with Ask Imbewu chat widget               │
│  - Story gallery                                           │
│  - Knowledge garden visualization                          │
└────────────────────┬────────────────────────────────────────┘
                     │ HTTPS
                     ▼
┌─────────────────────────────────────────────────────────────┐
│              SECURE BACKEND (Node.js/Python)                │
│                                                             │
│  - API endpoint: /api/imbewu/chat                          │
│  - Session management                                      │
│  - Credential management (API keys secure)                │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────────┐
│         MICROSOFT FOUNDRY AGENT SERVICE                      │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ Imbewu AI Agent                                      │  │
│  │ - Multi-turn conversation                           │  │
│  │ - Context Collection                                │  │
│  │ - Classification                                    │  │
│  │ - System prompt (cultural respect guidelines)       │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌──────────────┐      ┌──────────────┐                    │
│  │ Azure AI     │      │ Knowledge    │                    │
│  │ Search       │      │ Base API     │                    │
│  │ (Retrieval)  │      │ (Community   │                    │
│  │              │      │  knowledge)  │                    │
│  └──────────────┘      └──────────────┘                    │
└──────────────────────────────────────────────────────────────┘
```

---

## Components

### 1. Frontend: Imbewu Learn UI

**File:** `imbewu-learn-v2.html`

**Features:**
- Chat interface for context collection
- Conversation state management
- Context badges display
- Responsive design

**Key Functions:**
- `sendMessage()` - Send user input
- `addMessage()` - Display conversation
- `updateContext()` - Show collected context
- `askImbewu()` - Scroll to chat section

---

### 2. Backend API

**Purpose:** Keep API credentials secure, never expose in client code

**Endpoint:** `POST /api/imbewu/chat`

**Request:**
```json
{
  "message": "How do I make umgombodi?",
  "sessionId": "user-session-123",
  "conversationHistory": [
    { "role": "user", "content": "How do I make umgombodi?" },
    { "role": "assistant", "content": "Thank you. Which culture or community does your knowledge come from?" }
  ]
}
```

**Response:**
```json
{
  "reply": "Imbewu AI: Thank you. Which culture or community does your knowledge come from?",
  "context": {
    "question": "How do I make umgombodi?",
    "culture": null,
    "location": null,
    "language": null,
    "experience": null
  },
  "stage": "getting_culture",
  "sessionId": "user-session-123"
}
```

---

### 3. Microsoft Foundry Agent

**Model:** GPT-4 or GPT-4-turbo

**System Prompt:**

```
You are Imbewu Learn, a culturally respectful learning assistant.

Your purpose is to help people learn community knowledge while respecting
differences between cultures, communities, families, and regions.

CORE PRINCIPLES:
1. Before answering a culturally specific question, determine whether you 
   know enough about the user's context.
2. Do NOT assume culture from geographic location.
3. Do NOT treat a culture as having one universal practice.
4. If important context is missing, ask a concise follow-up question.

CONTEXT COLLECTION:
Collect these fields when relevant:
- Culture/community (e.g., isiZulu, Sepedi, isiXhosa, Xitsonga)
- Geographic region (e.g., KwaZulu-Natal, Gauteng, Western Cape)
- Preferred language (e.g., English, isiZulu, Sepedi, isiXhosa)
- Experience level (Beginner, Intermediate, Advanced)
- Learning goal (What are they trying to accomplish?)

CLASSIFICATION:
Analyze the query to produce a classification like:
{
  "topic": "traditional_food",
  "subtopic": "umgombodi",
  "culture": "isiZulu",
  "region": "KwaZulu-Natal",
  "language": "isiZulu",
  "experience": "beginner",
  "needs_follow_up": true,
  "confidence": 0.91
}

RESPONSE GUIDELINES:
- Prefer verified community knowledge from the Imbewu knowledge base.
- Clearly distinguish documented knowledge from general AI knowledge.
- If practices differ between communities, explain the differences.
- Do not invent traditions or cultural practices.
- Respect the user's preferred language.
- Adapt explanations to the user's experience level.
- If the knowledge base lacks information, say so.
- Never claim that one community's practice is the only correct practice.

CONVERSATION FLOW:
1. User asks a question about community knowledge
2. Determine if you have enough context
3. If not, ask ONE concise follow-up question
4. Repeat until you have sufficient context
5. Retrieve from knowledge base
6. Provide culturally grounded, personalized answer

Your goal is not to simply answer questions.
Your goal is to help the learner understand knowledge in the context 
in which it belongs.
```

**Tools/Connections:**
- Azure AI Search (for knowledge base retrieval)
- Community Knowledge Base API

---

### 4. Knowledge Base Structure

The knowledge base will eventually contain:

```
Knowledge Base
├── Food & Cooking
│   ├── Umgombodi
│   │   ├── isiZulu
│   │   │   ├── Traditional preparation (family-based)
│   │   │   ├── Regional variations (KwaZulu-Natal)
│   │   │   └── Ceremonial uses
│   │   ├── Sepedi
│   │   │   ├── Similar dishes
│   │   │   └── Preparation differences
│   │   └── isiXhosa
│   │       └── Preparation variations
│   │
│   ├── Umqombothi (Traditional beer)
│   │   ├── isiZulu brewing techniques
│   │   ├── Ceremonial significance
│   │   └── Community practices
│   │
│   └── Mealies preparation
│       ├── Planting (Zulu tradition)
│       ├── Storage (seasonal knowledge)
│       └── Community harvest practices
│
├── Traditional Agriculture
│   ├── Crop rotation (community knowledge)
│   ├── Seasonal indicators
│   └── Indigenous plant uses
│
├── Medicinal Plants
│   ├── Healing traditions by culture
│   ├── Preparation methods
│   └── Community testimonials
│
├── Traditional Crafts
│   ├── Basket weaving
│   ├── Beadwork
│   └── Cultural significance
│
└── Ceremonies & Storytelling
    ├── Coming-of-age rituals
    ├── Family traditions
    └── Oral history preservation
```

---

## Implementation Steps

### Phase 1: Frontend (✅ Completed)
- [x] Add Ask Imbewu UI to landing page
- [x] Implement client-side context collection
- [x] Local conversation state management
- [ ] Connect to backend API

### Phase 2: Backend (In Progress)
- [ ] Create Node.js/Python backend
- [ ] Implement API endpoint
- [ ] Session management
- [ ] Error handling & logging

### Phase 3: AI Integration
- [ ] Set up Microsoft Foundry workspace
- [ ] Configure Imbewu Agent
- [ ] Implement system prompt
- [ ] Set up Azure AI Search

### Phase 4: Knowledge Base
- [ ] Design knowledge structure
- [ ] Implement knowledge API
- [ ] Populate initial content
- [ ] Search & retrieval system

---

## Security Best Practices

### ✅ DO:
- Store API keys in backend environment variables
- Use HTTPS for all API calls
- Implement rate limiting on backend
- Validate all user inputs
- Log conversations for moderation
- Use session tokens (not API keys in frontend)

### ❌ DON'T:
- Put Azure API keys in client-side code
- Expose Foundry credentials in GitHub
- Trust client-side context validation
- Store sensitive user data in browser

---

## Deploying the Frontend

The frontend can be deployed on GitHub Pages:

```bash
# Option 1: Deploy imbewu-learn-v2.html as index.html
cp imbewu-learn-v2.html index.html
git add index.html
git commit -m "Deploy updated landing page with Ask Imbewu"
git push origin master
```

The page will be available at: `https://yanele22.github.io/Imbewu-Learn/`

---

## Deploying the Backend

**Option 1: Heroku (Node.js)**
```bash
# Create Procfile
echo "web: node server.js" > Procfile

# Deploy
heroku login
heroku create imbewu-learn-api
git push heroku main
```

**Option 2: Azure App Service**
```bash
az webapp up --name imbewu-learn-api --runtime "NODE|18-lts"
```

**Option 3: Local Development**
```bash
npm install
npm start
# API runs at http://localhost:3000
```

---

## Key Differentiators

This architecture makes Imbewu Learn unique:

1. **Context-First Approach**: Unlike generic chatbots, Imbewu asks about cultural context BEFORE answering

2. **Community Knowledge**: Doesn't invent - retrieves from community-contributed knowledge

3. **Cultural Respect**: Acknowledges that practices vary between families and communities, never assumes

4. **Personalized Learning Paths**: Same question = different answers based on learner's context

5. **Secure by Design**: API keys never exposed in client code

---

## Future Enhancements

- Voice input (transcribe to text using Azure Speech)
- Multi-language support (translate questions and responses)
- Offline mode (cache frequently accessed knowledge)
- Mobile app (React Native)
- Knowledge contributor dashboard
- Community moderation system
- Learning progress tracking
- Certificates/badges for completed lessons

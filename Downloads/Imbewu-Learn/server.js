/**
 * Imbewu Learn Backend API
 * 
 * This backend securely handles requests to Microsoft Foundry.
 * API keys are stored server-side and never exposed to clients.
 * 
 * Setup:
 *   1. npm install express dotenv axios cors
 *   2. Create .env file with AZURE_FOUNDRY_KEY and AZURE_FOUNDRY_ENDPOINT
 *   3. npm start
 */

const express = require('express');
const cors = require('cors');
const axios = require('axios');
require('dotenv').config();

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors({
  origin: [
    'http://localhost:3000',
    'https://yanele22.github.io',
    'https://yanele22.github.io/Imbewu-Learn'
  ]
}));
app.use(express.json());

// Session storage (use Redis in production)
const sessions = new Map();

// Imbewu System Prompt
const IMBEWU_SYSTEM_PROMPT = `You are Imbewu Learn, a culturally respectful learning assistant.

Your purpose is to help people learn community knowledge while respecting
differences between cultures, communities, families, and regions.

CORE PRINCIPLES:
1. Before answering a culturally specific question, determine whether you 
   know enough about the user's context.
2. Do NOT assume culture from geographic location.
3. Do NOT treat a culture as having one universal practice.
4. If important context is missing, ask ONE concise follow-up question.

CONTEXT COLLECTION (when relevant):
- Culture/community (e.g., isiZulu, Sepedi, isiXhosa, Xitsonga)
- Geographic region (e.g., KwaZulu-Natal, Gauteng, Western Cape)
- Preferred language (e.g., English, isiZulu, Sepedi, isiXhosa)
- Experience level (Beginner, Intermediate, Advanced)
- Learning goal (What are they trying to accomplish?)

RESPONSE GUIDELINES:
- Prefer verified community knowledge from the Imbewu knowledge base.
- Clearly distinguish documented knowledge from general AI knowledge.
- If practices differ between communities, explain the differences.
- Do not invent traditions or cultural practices.
- Respect the user's preferred language.
- Adapt explanations to the user's experience level.
- If the knowledge base lacks information, say so clearly.
- Never claim that one community's practice is the only correct practice.

CONVERSATION STRATEGY:
1. Assess if you have enough context
2. If not, ask ONE focused follow-up question about missing context
3. Repeat until you have sufficient context (usually 3-5 exchanges)
4. Then provide a personalized, culturally-grounded answer
5. Clearly separate community-specific knowledge from general information

Remember: Your goal is to help the learner understand knowledge in the 
context in which it belongs.`;

/**
 * Main endpoint: Chat with Imbewu AI
 * POST /api/imbewu/chat
 */
app.post('/api/imbewu/chat', async (req, res) => {
  try {
    const { message, sessionId, conversationHistory } = req.body;

    if (!message) {
      return res.status(400).json({ error: 'Message is required' });
    }

    if (!sessionId) {
      return res.status(400).json({ error: 'Session ID is required' });
    }

    // Get or create session
    let session = sessions.get(sessionId);
    if (!session) {
      session = {
        conversationHistory: [],
        context: {},
        stage: 'initial'
      };
      sessions.set(sessionId, session);
    }

    // Add user message to history
    session.conversationHistory.push({
      role: 'user',
      content: message
    });

    // For demo: use local implementation without Foundry
    // In production: call Microsoft Foundry API
    const aiResponse = await generateAimbewuResponse(
      message,
      session.conversationHistory,
      session.context
    );

    // Add AI response to history
    session.conversationHistory.push({
      role: 'assistant',
      content: aiResponse.reply
    });

    // Update session context
    Object.assign(session.context, aiResponse.context);
    session.stage = aiResponse.stage;

    // Send response
    res.json({
      reply: aiResponse.reply,
      context: session.context,
      stage: session.stage,
      sessionId: sessionId
    });

  } catch (error) {
    console.error('Error in /api/imbewu/chat:', error);
    res.status(500).json({
      error: 'Failed to process request',
      message: error.message
    });
  }
});

/**
 * Demo implementation of Imbewu responses
 * In production, replace with Microsoft Foundry API call
 */
async function generateAimbewuResponse(message, history, context) {
  // Simple state machine for demo
  const stage = determineConversationStage(context);
  
  let reply = '';
  const updatedContext = { ...context };

  if (stage === 'initial' && !context.question) {
    // First time - capture question
    updatedContext.question = message;
    reply = '🌱 Imbewu AI: Thank you. Which culture or community does your knowledge come from? (E.g., isiZulu, Sepedi, isiXhosa, Xitsonga)';
    return {
      reply,
      context: updatedContext,
      stage: 'getting_culture'
    };
  }

  if (stage === 'getting_culture' && !context.culture) {
    updatedContext.culture = message;
    reply = '🌱 Imbewu AI: Which region or area are you from? Practices can vary between communities. (E.g., KwaZulu-Natal, Gauteng, Western Cape)';
    return {
      reply,
      context: updatedContext,
      stage: 'getting_location'
    };
  }

  if (stage === 'getting_location' && !context.location) {
    updatedContext.location = message;
    reply = '🌱 Imbewu AI: Which language would you prefer to learn in? (E.g., English, isiZulu, Sepedi, isiXhosa)';
    return {
      reply,
      context: updatedContext,
      stage: 'getting_language'
    };
  }

  if (stage === 'getting_language' && !context.language) {
    updatedContext.language = message;
    reply = '🌱 Imbewu AI: What is your experience level with this topic? (Beginner, Intermediate, or Advanced)';
    return {
      reply,
      context: updatedContext,
      stage: 'getting_experience'
    };
  }

  if (stage === 'getting_experience' && !context.experience) {
    updatedContext.experience = message;
    
    // Generate personalized response
    const response = `🌱 Imbewu AI: Perfect! Based on what you've shared:

**Your Context:**
• Culture: ${updatedContext.culture}
• Region: ${updatedContext.location}
• Language: ${updatedContext.language}
• Level: ${updatedContext.experience}
• Question: "${updatedContext.question}"

I will tailor the explanation to your context. I'll:
✓ Use knowledge specific to ${updatedContext.culture} traditions
✓ Reference practices from ${updatedContext.location}
✓ Present information in ${updatedContext.language}
✓ Match explanations to your ${updatedContext.experience} level
✓ Clearly separate community-specific knowledge from general information
✓ Acknowledge that practices vary between families and communities

**Note:** This is a demo interface. In production, I would connect to our knowledge base and use Microsoft Foundry to provide personalized, culturally-grounded responses from verified community knowledge.

Would you like me to proceed with explaining "${updatedContext.question}" in this context?`;
    
    reply = response;
    return {
      reply,
      context: updatedContext,
      stage: 'complete'
    };
  }

  // If already complete, continue conversation
  reply = `🌱 Imbewu AI: Thank you for that input. I'm here to help. In the production system, I would search our knowledge base for community knowledge about "${context.question}" and present it tailored to your ${context.culture} cultural context from ${context.location}.`;
  
  return {
    reply,
    context: updatedContext,
    stage: 'complete'
  };
}

function determineConversationStage(context) {
  if (!context.experience) return 'getting_experience';
  if (!context.language) return 'getting_language';
  if (!context.location) return 'getting_location';
  if (!context.culture) return 'getting_culture';
  if (!context.question) return 'initial';
  return 'complete';
}

/**
 * Health check endpoint
 */
app.get('/health', (req, res) => {
  res.json({ status: 'ok', service: 'Imbewu Learn API' });
});

/**
 * Clear session endpoint (for testing)
 */
app.post('/api/session/clear', (req, res) => {
  const { sessionId } = req.body;
  if (sessionId) {
    sessions.delete(sessionId);
    res.json({ message: 'Session cleared' });
  } else {
    sessions.clear();
    res.json({ message: 'All sessions cleared' });
  }
});

/**
 * Start server
 */
app.listen(PORT, () => {
  console.log(`
╔════════════════════════════════════════╗
║   Imbewu Learn API Server Started      ║
║                                        ║
║   🌱 Ask Imbewu API is running         ║
║   Port: ${PORT}                             ║
║   Endpoint: http://localhost:${PORT}/api/imbewu/chat  ║
║                                        ║
║   Health: http://localhost:${PORT}/health          ║
╚════════════════════════════════════════╝
  `);
});

// Graceful shutdown
process.on('SIGTERM', () => {
  console.log('SIGTERM received. Shutting down gracefully...');
  process.exit(0);
});

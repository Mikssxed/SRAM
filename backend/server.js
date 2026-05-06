const express = require('express');
const http = require('http');
const { Server } = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = new Server(server, { cors: { origin: "*" } });

app.use(express.json());

// In-memory storage
let restroomStatus = { "men1": false };

// NEW: Array to store the history with timestamps
let statusHistory = [];
const MAX_HISTORY = 100; // Keep only the latest 100 entries to save RAM

// 1. Endpoint for the ESP32 to push data
app.post('/api/status', (req, res) => {
  const { id, occupied } = req.body;
  
  if (id && occupied !== undefined) {
    // Only save to history if the status actually changed
    if (restroomStatus[id] !== occupied) {
      restroomStatus[id] = occupied;
      
      // Create a history entry with a timestamp
      const historyEntry = {
        id: id,
        occupied: occupied,
        timestamp: new Date().toISOString() // Returns format like: "2023-10-25T14:30:00.000Z"
      };
      
      statusHistory.push(historyEntry);
      
      // Prevent the array from growing forever
      if (statusHistory.length > MAX_HISTORY) {
        statusHistory.shift(); // Remove the oldest entry
      }

      console.log(`[${historyEntry.timestamp}] Restroom ${id} is now ${occupied ? 'Occupied' : 'Free'}`);
    }
    
    // Broadcast the change to the live Frontend
    io.emit('statusUpdate', restroomStatus);
    res.status(200).send('OK');
  } else {
    res.status(400).send('Bad Request');
  }
});

// NEW: 2. Endpoint to fetch all saved results with timestamps
app.get('/api/history', (req, res) => {
  // Returns the whole array as JSON
  res.json(statusHistory); 
});

// 3. Send current state to new frontend connections
io.on('connection', (socket) => {
  console.log('Frontend dashboard connected');
  socket.emit('statusUpdate', restroomStatus);
});

// Start the server
server.listen(3000, '0.0.0.0', () => {
  console.log('Backend running on port 3000');
});
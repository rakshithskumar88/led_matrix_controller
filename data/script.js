// DOM Elements
const tabButtons = document.querySelectorAll('.tab-btn');
const tabContents = document.querySelectorAll('.tab-content');
const patternList = document.querySelector('.pattern-list');
const patternCreateForm = document.getElementById('pattern-create-form');
const addStageButton = document.getElementById('add-stage');
const stagesContainer = document.getElementById('stages-container');
const stageTemplate = document.getElementById('stage-template');

// Predefined patterns (matching Arduino patterns)
const predefinedPatterns = [
    "Aurora Cascade",
    "Neon Pulse",
    "Digital Rain",
    "Mystic Waves",
    "Rainbow Flow",
    "Starlight Twinkle",
    "Ocean Waves",
    "Fire Dance",
    "Matrix Code",
    "Heartbeat",
    "Color Symphony",
    "Binary Counter",
    "Gentle Breeze",
    "Northern Lights",
    "Cyber Pulse",
    "Rainbow Chase",
    "Morse Code SOS",
    "Fibonacci Sequence",
    "Color Meditation",
    "Quantum Entanglement"
];

// Initialize the UI
function initializeUI() {
    // Populate pattern list
    predefinedPatterns.forEach((pattern, index) => {
        const div = document.createElement('div');
        div.className = 'pattern-option';
        div.innerHTML = `
            <input type="radio" name="pattern" value="${index}" id="pattern-${index}">
            <label for="pattern-${index}">${pattern}</label>
        `;
        patternList.appendChild(div);
    });

    // Add first stage to pattern creator
    addStage();
}

// Tab switching
tabButtons.forEach(button => {
    button.addEventListener('click', () => {
        tabButtons.forEach(btn => btn.classList.remove('active'));
        tabContents.forEach(content => content.classList.remove('active'));
        
        button.classList.add('active');
        const tabId = button.dataset.tab;
        document.getElementById(tabId).classList.add('active');
    });
});

// Pattern selection handling
document.getElementById('pattern-select-form').addEventListener('change', async (e) => {
    if (e.target.type === 'radio') {
        try {
            const response = await fetch(`/select?pattern=${e.target.value}`);
            if (!response.ok) throw new Error('Failed to select pattern');
            showNotification('Pattern selected successfully');
        } catch (error) {
            showNotification('Error selecting pattern', true);
        }
    }
});

// Stage management
function addStage() {
    const stageCount = stagesContainer.children.length;
    const stageNode = stageTemplate.content.cloneNode(true);
    
    // Update stage number
    stageNode.querySelector('h3').textContent = `Stage ${stageCount + 1}`;
    
    // Setup circular sliders
    stageNode.querySelectorAll('.slider').forEach(slider => {
        setupCircularSlider(slider);
    });
    
    stagesContainer.appendChild(stageNode);
}

addStageButton.addEventListener('click', addStage);

// Circular slider setup
function setupCircularSlider(slider) {
    const valueDisplay = slider.parentElement.querySelector('.slider-value');
    
    // Update value display
    slider.addEventListener('input', () => {
        valueDisplay.textContent = slider.value;
        updateSliderRotation(slider);
    });
    
    // Initial rotation
    updateSliderRotation(slider);
}

function updateSliderRotation(slider) {
    const rotation = (slider.value / 255) * 360;
    slider.style.transform = `translate(-50%, -50%) rotate(${rotation}deg)`;
}

// Pattern creation handling
patternCreateForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const patternName = document.getElementById('pattern-name').value;
    const stages = [];
    
    // Collect all stage data
    document.querySelectorAll('.stage').forEach(stage => {
        const brightness = [];
        stage.querySelectorAll('.slider').forEach(slider => {
            brightness.push(parseInt(slider.value));
        });
        stages.push(brightness);
    });
    
    const patternData = {
        name: patternName,
        stages: stages
    };
    
    try {
        const response = await fetch('/save', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(patternData)
        });
        
        if (!response.ok) throw new Error('Failed to save pattern');
        
        showNotification('Pattern saved successfully');
        patternCreateForm.reset();
        stagesContainer.innerHTML = '';
        addStage(); // Add initial stage
        
    } catch (error) {
        showNotification('Error saving pattern', true);
    }
});

// Notification system
function showNotification(message, isError = false) {
    const notification = document.createElement('div');
    notification.className = `notification ${isError ? 'error' : 'success'}`;
    notification.textContent = message;
    
    document.body.appendChild(notification);
    
    // Trigger animation
    setTimeout(() => notification.classList.add('show'), 10);
    
    // Remove notification
    setTimeout(() => {
        notification.classList.remove('show');
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

// Initialize UI when DOM is loaded
document.addEventListener('DOMContentLoaded', initializeUI);

// Helper function to prevent slider from jumping when dragged
document.addEventListener('mousemove', (e) => {
    if (e.target.classList.contains('slider')) {
        e.preventDefault();
    }
});

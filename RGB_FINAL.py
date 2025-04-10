import cv2
import mediapipe as mp
import serial
import time
import numpy as np

# Initialize Serial Communication using USB to send input
arduino = serial.Serial(port="COM5", baudrate=9600, timeout=1)
time.sleep(2)

# Initialize MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=2, min_detection_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

# Webcam
cap = cv2.VideoCapture(0)
grid_state = np.zeros((8, 8), dtype=int)  # Only track active cells
hand_labels = {}

def map_to_grid(x, y):
    x_grid = min(max(int(x * 8), 0), 7)
    y_grid = min(max(int(y * 8), 0), 7)
    return x_grid, y_grid

def draw_grid_overlay(frame, grid_state):
    h, w, _ = frame.shape
    cell_size = min(h, w) // 8

    start_x = (w - (cell_size * 8)) // 2
    start_y = (h - (cell_size * 8)) // 2

    for i in range(8):
        for j in range(8):
            top_left = (start_x + j * cell_size, start_y + i * cell_size)
            bottom_right = (start_x + (j + 1) * cell_size, start_y + (i + 1) * cell_size)

            # Activated Cell
            if grid_state[i][j]:  
                cv2.rectangle(frame, top_left, bottom_right, (0, 255, 0), -1)

            # White grid lines
            cv2.rectangle(frame, top_left, bottom_right, (255, 255, 255), 1)

while cap.isOpened():
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_frame)

    if results.multi_hand_landmarks and results.multi_handedness:
        for i, hand_landmarks in enumerate(results.multi_hand_landmarks):
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            hand_label = results.multi_handedness[i].classification[0].label  # Left or Right Hand
            hand_labels[hand_label] = hand_landmarks

        # Index Fingers
        for hand, label in hand_labels.items():
            index_tip = label.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            x_grid, y_grid = map_to_grid(index_tip.x, index_tip.y)
            
            if grid_state[y_grid][x_grid] == 0:  # Activate cell if not already active
                grid_state[y_grid][x_grid] = 1
                arduino.write(f"{x_grid},{y_grid},{hand}\n".encode())

    draw_grid_overlay(frame, grid_state) 
    cv2.imshow("Finger Drawing", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
arduino.close()
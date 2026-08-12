from flask import Flask, render_template, jsonify
import requests
import sqlite3
import pandas as pd
from datetime import datetime, timedelta
import os

app = Flask(__name__)

# NodeMCU IP Address
# IMPORTANT: Since NodeMCU is now in Station mode, check your Serial Monitor 
# for the IP address assigned by your router and update it here.
NODEMCU_IP = "YOUR_NODEMCU_IP" 

# --- DATABASE SETUP ---
DB_PATH = "sensor_data.db"
last_storage_time = datetime.min

def init_db():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    # Raw data for real-time tracking
    c.execute('''CREATE TABLE IF NOT EXISTS air_quality 
                 (timestamp DATETIME, temp REAL, hum REAL, pm25 REAL, aqi INTEGER, gas INTEGER, lat REAL, lng REAL)''')
    # Hourly averages for long-term prediction
    c.execute('''CREATE TABLE IF NOT EXISTS hourly_averages 
                 (timestamp DATETIME, avg_aqi REAL)''')
    conn.commit()
    conn.close()

init_db()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/roadside')
def roadside():
    return render_template('roadside.html')

@app.route('/get_comparison')
def get_comparison():
    conn = sqlite3.connect(DB_PATH)
    # Today's average
    today_start = datetime.now().replace(hour=0, minute=0, second=0, microsecond=0)
    # Yesterday's average
    yesterday_start = today_start - timedelta(days=1)
    
    today_avg = pd.read_sql_query("SELECT AVG(pm25) as avg FROM air_quality WHERE timestamp >= ?", conn, params=(today_start,)).iloc[0]['avg']
    yesterday_avg = pd.read_sql_query("SELECT AVG(pm25) as avg FROM air_quality WHERE timestamp >= ? AND timestamp < ?", conn, params=(yesterday_start, today_start)).iloc[0]['avg']
    
    conn.close()
    return jsonify({
        "today": round(float(today_avg) if today_avg else 0, 2),
        "yesterday": round(float(yesterday_avg) if yesterday_avg else 0, 2)
    })

@app.route('/get_data')
def get_data():
    try:
        response = requests.get(f"http://{NODEMCU_IP}/data", timeout=2)
        if response.status_code == 200:
            data = response.json()
            # Log data to DB every 30 minutes
            global last_storage_time
            now = datetime.now()
            conn = sqlite3.connect(DB_PATH)
            c = conn.cursor()
            
            if now - last_storage_time >= timedelta(minutes=5):
                c.execute("INSERT INTO air_quality VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                          (now, data['temp'], data['hum'], data['pm25'], data.get('aqi', 0), data['gas'], data['lat'], data['lng']))
                last_storage_time = now
            
            # Hourly Aggregation Logic: Store average if hour changed
            last_hour = now.replace(minute=0, second=0, microsecond=0)
            c.execute("SELECT timestamp FROM hourly_averages WHERE timestamp = ?", (last_hour,))
            if not c.fetchone():
                # Calculate average for the previous hour
                prev_hour_start = last_hour - timedelta(hours=1)
                c.execute("SELECT AVG(aqi) FROM air_quality WHERE timestamp >= ? AND timestamp < ?", 
                          (prev_hour_start, last_hour))
                avg_aqi = c.fetchone()[0]
                if avg_aqi:
                    c.execute("INSERT INTO hourly_averages VALUES (?, ?)", (prev_hour_start, avg_aqi))
            
            conn.commit()
            conn.close()
            return jsonify(data)
        else:
            return jsonify({"error": "Failed to fetch data"}), 500
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/export_csv')
def export_csv():
    conn = sqlite3.connect(DB_PATH)
    df = pd.read_sql_query("SELECT * FROM air_quality", conn)
    conn.close()
    
    csv_path = "daily_data.csv"
    df.to_csv(csv_path, index=False)
    return jsonify({"message": "Data exported", "file": csv_path})

@app.route('/predict')
def predict():
    conn = sqlite3.connect(DB_PATH)
    # Use hourly averages for stable prediction
    df = pd.read_sql_query("SELECT avg_aqi as val FROM hourly_averages ORDER BY timestamp DESC LIMIT 24", conn)
    
    # Fallback to raw data if hourly logs are empty (new system)
    if df.empty:
        df = pd.read_sql_query("SELECT aqi as val FROM air_quality ORDER BY timestamp DESC LIMIT 100", conn)
    
    conn.close()
    
    if len(df) < 3:
        return jsonify({"prediction": "Collecting initial hourly data..."})
    
    import random
    
    current_val = df['val'].iloc[0]
    
    # Calculate Slope (Rate of Change) over the available data
    if len(df) > 1:
        # Note: df is DESC, so [0] is newest, [-1] is oldest
        total_change = df['val'].iloc[0] - df['val'].iloc[-1]
        slope = total_change / len(df)
    else:
        slope = 0
    
    # Forecasting based on the slope + temporal variance + small seasonal flux
    # Week: short term minor fluctuation
    week_f = current_val + (slope * 7) + random.uniform(-0.5, 0.5)
    
    # Month: mid term, add a small 2% environmental factor
    month_f = current_val + (slope * 30) + random.uniform(-2.0, 2.0) + (current_val * 0.02)
    
    # Year: long term, add a 5% environmental factor
    year_f = current_val + (slope * 365) + random.uniform(-5.0, 5.0) + (current_val * 0.05)

    # Ensure values don't go negative or exceed AQI scale
    week_f = max(0, min(500, week_f))
    month_f = max(0, min(500, month_f))
    year_f = max(0, min(500, year_f))

    trend = "Stable"
    if slope > 0.05: trend = "Increasing"
    elif slope < -0.05: trend = "Decreasing"

    return jsonify({
        "current_avg": round(float(current_val), 2),
        "trend": trend,
        "week_forecast": round(float(week_f), 2),
        "month_forecast": round(float(month_f), 2),
        "year_forecast": round(float(year_f), 2)
    })

@app.route('/get_recent_data')
def get_recent_data():
    conn = sqlite3.connect(DB_PATH)
    # Get last 10 logs
    df = pd.read_sql_query("SELECT timestamp, pm25, aqi, gas, temp, lat, lng FROM air_quality ORDER BY timestamp DESC LIMIT 10", conn)
    conn.close()
    return jsonify(df.to_dict(orient='records'))

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)

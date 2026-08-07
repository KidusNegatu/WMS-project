import { Client, TablesDB } from "https://cdn.jsdelivr.net/npm/appwrite@21.0.0/+esm";

const dashboardSection = document.getElementById("dashboardSection");
const historySection = document.getElementById("historySection");
const statisticsSection = document.getElementById("statisticsSection");
const aboutSection = document.getElementById("aboutSection");
const sideBar = document.getElementById("sideBar");
const dBtn = document.getElementById("dBtn");
const sBtn = document.getElementById("sBtn")
const hBtn = document.getElementById("hBtn");
const aBtn = document.getElementById("aBtn");
dBtn.addEventListener("click", () => {
    dashboardSection.style.display = "block";
    historySection.style.display = "none";
    aboutSection.style.display = "none";
    statisticsSection.style.display = "none";
});
sBtn.addEventListener("click", () => {
    dashboardSection.style.display = "none";
    statisticsSection.style.display = "block";
    historySection.style.display = "none";
    aboutSection.style.display = "none";
});

hBtn.addEventListener("click", () => {
    historySection.style.display = "block";
    dashboardSection.style.display = "none";
    aboutSection.style.display = "none";
    statisticsSection.style.display = "none";
});
aBtn.addEventListener("click", () => {
    aboutSection.style.display = "block";
    dashboardSection.style.display = "none";
    historySection.style.display = "none";
    statisticsSection.style.display = "none";
});

const menu = document.getElementById("menus");
let menuBool = false;
menu.addEventListener("click", () => {
    if(menuBool) {
        sideBar.style.display = "block";
        sideBar.style.position = "fixed";
        sideBar.style.zIndex = 1000;
        sideBar.style.height = "100vh";
        menu.innerHTML = '<ion-icon name="close-outline"></ion-icon>';
        menuBool = !menuBool;
    }
    else {
        sideBar.style.display = "none";
        menu.innerHTML = '<ion-icon name="menu-outline"></ion-icon>';
        menuBool = true;
    };
});

const tempData = document.getElementById("tempData");
const humidityData = document.getElementById("humidityData");
const rainData = document.getElementById("rainData");
const lightData = document.getElementById("lightData");
const airPresData = document.getElementById("airPresData");
const altitudeData = document.getElementById("altitudeData");
const windData = document.getElementById("windData");
const iconBox = document.getElementById("iconBox");
const conditionText = document.getElementById("conditionText");
const statTemp = document.getElementById("statTemp");
const statFeelsTemp = document.getElementById("statFeelsTemp");
const statMaxTemp = document.getElementById("statMaxTemp");
const statMinTemp = document.getElementById("statMinTemp");
const statAvgTemp = document.getElementById("statAvgTemp");
const statHumid = document.getElementById("statHumid");
const statAvgHumid = document.getElementById("statAvgHumid");
const statAirPress = document.getElementById("statAirPress");
const statAlti = document.getElementById("statAlti");
const statLDR = document.getElementById("statLDR");
const statCloud = document.getElementById("statCloud");
const statChanceRain = document.getElementById("statChanceRain");
const statRain = document.getElementById("statRain");
const statSunrise = document.getElementById("statSunrise");
const statSunset = document.getElementById("statSunset");
const statWind = document.getElementById("statWind");

async function appWriteDataProcess() {
  const user = new Client();
  user.setEndpoint("https://nyc.cloud.appwrite.io/v1").setProject("42914291");
  const appWriteTable = new TablesDB(user);
  let res = await appWriteTable.listRows({
    databaseId: "6a73a050001026525006",
    tableId: "wms_database"
  });
  console.log(res);
  let wholeData = res.rows
  const latestRow = res.rows[res.rows.length - 1];
  const {
    lastUpdated,
    Temp,
    Humidity,
    LDRValue,
    dayOrNight,
    cloudCover,
    feelsLikeTemp,
    windKPH,
    windDirection,
    maxTemp,
    minTemp,
    avgTemp,
    condition,
    chanceOfRain,
    avgHumidity,
    maxWindKph,
    sunrise,
    sunset,
    conditionIcon,
    Alert,
    $createdAt,
    $updatedAt,
    $id,
    RainValue,
    AirPressure,
    Altitude,
    Icon 
  } = latestRow;
  console.log(Temp);
  tempData.innerText = String(Temp) + "°C" ?? "N/A";
  humidityData.innerText = String(Humidity) + "%" ?? "N/A";
  rainData.innerText = String(RainValue) + "%" ?? "N/A";
  lightData.innerText = String(LDRValue) + "%" ?? "N/A";
  airPresData.innerText = String(AirPressure) + "hpa" ?? "N/A";
  altitudeData.innerText = String(Altitude) ?? "N/A";
  windData.innerText = String(windKPH) + "kph"?? "N/A";
  iconBox.innerHTML = Icon ?? '<ion-icon name="ban-outline"></ion-icon>';
  conditionText.innerText = condition ?? "N/A"
  
  const API_KEY = "e798e2ccd8608c3cc1004054bd36de81";
  const latitude = 9.03;
  const longitude = 38.74;
  const map = L.map("map").setView(
      [
          latitude,
          longitude
      ],
      6
  );
  const baseMap = L.tileLayer(
    "https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png",
    {
        attribution:
        '&copy; OpenStreetMap &copy; CARTO'
    }
  );
  baseMap.addTo(map);
  const rainLayer = L.tileLayer(
  
      `https://tile.openweathermap.org/map/precipitation_new/{z}/{x}/{y}.png?appid=${API_KEY}`,
  
      {
          opacity: 0.6
      }
  
  );
  const temperatureLayer = L.tileLayer(
  
      `https://tile.openweathermap.org/map/temp_new/{z}/{x}/{y}.png?appid=${API_KEY}`,
  
      {
          opacity: 0.6
      }
  
  );
  const cloudLayer = L.tileLayer(
  
      `https://tile.openweathermap.org/map/clouds_new/{z}/{x}/{y}.png?appid=${API_KEY}`,
  
      {
          opacity: 0.6
      }
  
  );
  const windLayer = L.tileLayer(
  
      `https://tile.openweathermap.org/map/wind_new/{z}/{x}/{y}.png?appid=${API_KEY}`,
  
      {
          opacity: 0.6
      }
  
  );
  rainLayer.addTo(map);
  const marker = L.marker(
      [
          latitude,
          longitude
      ]
  )
  .addTo(map);
  marker.bindPopup(
    `
    <div class="weather-popup">
    
        <h3>🌦 WMS Station</h3>
    
        <div class="data">
            Temperature:
            <span>${Temp}°C</span>
        </div>
    
        <div class="data">
            Humidity:
            <span>${Humidity}%</span>
        </div>
    
        <div class="data">
            Pressure:
            <span>${AirPressure} hPa</span>
        </div>
        <div class="data">
            Wind:
            <span>${windKPH} KPH</span>
        </div>
    </div>
    `
);
  marker.openPopup();
  const weatherLayers = {
      "Rain": rainLayer,
      "Temperature": temperatureLayer,
      "Cloud": cloudLayer,
      "Wind": windLayer
  };
  L.control.layers(
      null,
      weatherLayers
  ).addTo(map);
  map.on(
      "click",
      function(e){
  
          console.log(
              "Latitude:",
              e.latlng.lat
          );
          console.log(
              "Longitude:",
              e.latlng.lng
          );
      }
  );
    statTemp.innerText = `${Temp}°C`;
    statFeelsTemp.innerText = `${feelsLikeTemp}°C`;
    statMaxTemp.innerText = `${maxTemp}°C`;
    statMinTemp.innerText = `${minTemp}°C`;
    statAvgTemp.innerText = `${avgTemp}°C`;
    statHumid.innerText = `${Humidity}%`;
    statAvgHumid.innerText = `${avgHumidity}%`;
    statAirPress.innerText = `${AirPressure} hPa`;
    statAlti.innerText = `${Altitude} m`;
    statLDR.innerText = `${LDRValue}`;
    statCloud.innerText = `${cloudCover}%`;
    statChanceRain.innerText = `${chanceOfRain}%`;
    statRain.innerText = `${RainValue} mm`;
    statSunrise.innerText = `${sunrise}`;
    statSunset.innerText = `${sunset}`;
    statWind.innerText = `${windKPH} km/h ${windDirection}`;
    if(dayOrNight == "Day time"){
      sideBar.style.background = "radial-gradient(ellipse 150% 100% at 5% 0%, #8d7546 0%, #a35b2b 10%, #803a17 20%, #4a2a1a 35%, #0d1620 55%, #010e1a 80%), linear-gradient(to bottom, #000a13, #010e1a)";
    }
    else{
      sideBar.style.background = "radial-gradient(ellipse 160% 110% at 8% 5%,     #5a6c84 0%,     #2e3c4e 10%,     #161f2a 25%,     #080c12 45%,     #000000 75%),linear-gradient(to bottom, #000000, #000000)";
    }
    const tabelBody = document.getElementById("tabelBody");

    tabelBody.innerHTML = ""; 
    wholeData.forEach((row) => {
      const dateSource = row.lastUpdated || row.$createdAt;
      const dateObj = new Date(dateSource);
      const years = dateObj.getFullYear();
      const months = String(dateObj.getMonth() + 1).padStart(2, "0");
      const days = String(dateObj.getDate()).padStart(2, "0");
      const hours = String(dateObj.getHours()).padStart(2, "0");
      const minutes = String(dateObj.getMinutes()).padStart(2, "0");
      const seconds = String(dateObj.getSeconds()).padStart(2, "0");
      const tr = document.createElement("tr");
      tr.innerHTML = `
        <td>${years}</td>
        <td>${months}</td>
        <td>${days}</td>
        <td>${hours}</td>
        <td>${minutes}</td>
        <td>${seconds}</td>
        <td>${row.Temp ?? "--"}°C</td>
        <td>${row.Humidity ?? "--"}%</td>
        <td>${row.RainValue ?? "--"}</td>
        <td>${row.LDRValue ?? "--"}</td>
        <td>${row.dayOrNight ?? "--"}</td>
        <td>${row.cloudCover ?? "--"}%</td>
        <td>${row.chanceOfRain ?? "--"}%</td>
        <td>${row.feelsLikeTemp ?? "--"}°C</td>
        <td>${row.maxTemp ?? "--"}°C</td>
        <td>${row.minTemp ?? "--"}°C</td>
        <td>${row.avgTemp ?? "--"}°C</td>
        <td>${row.windKPH ?? "--"}</td>
        <td>${row.windDirection ?? "--"}</td>
        <td>${row.condition ?? "--"}</td>
        <td>${row.avgHumidity ?? "--"}%</td>
        <td>${row.AirPressure ?? "--"}</td>
        <td>${row.Altitude ?? "--"}</td>
        <td>${row.sunset ?? "--"}</td>
        <td>${row.sunrise ?? "--"}</td>
      `;
      tabelBody.appendChild(tr);
    });
  };
appWriteDataProcess();

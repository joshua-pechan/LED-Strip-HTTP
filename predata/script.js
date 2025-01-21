const buttons = document.querySelectorAll("button");
const hueSlider = document.getElementById("hueSlider");
const colorDisplay = document.getElementById("colorDisplay");
const increasingHueToggle = document.getElementById("increasingHue");

const data = { 
    hue: -1,
    increasingHue: false,
    pattern: ""
}

const postRequest = (id, transferData) => {
    data[id] = transferData;
    fetch("/", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    })
    .then((response) => response.text())
    .catch((error) => console.error("Error:", error));
}

hueSlider.addEventListener("change", () => {
    postRequest("hue", hueSlider.value);
});

hueSlider.addEventListener("input", () => {
    colorDisplay.style.backgroundColor = `hsl(${hueSlider.value * (360/255)}, 100%, 50%)`;
});

increasingHueToggle.addEventListener("change", () => {
    postRequest("increasingHue", !data["increasingHue"]);
});

document.addEventListener("click", event => { buttons.forEach(button => {
    if (event.target == button) {
        postRequest("pattern", button.id);
    }});
});

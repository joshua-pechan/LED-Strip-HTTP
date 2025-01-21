# LED Strip HTTP Website
Allows for pattern and hue changes for an LED strip with a simple interface on a website hosted by a esp8266. This is for personal use as it will be hung up in diffusion channels on the outside of my house.

## Demo

Insert gif or link to demo


## Tech Stack

**Client:** littlefs (HTML, JS)

**Server:** espressif8266


## Optimizations
There was a need to Minify JS and HTML because the flash size was smaller than initially expected. Before this there was a more limited amount of code able to be on the device leading to it being cut off in runtime if it was too long.

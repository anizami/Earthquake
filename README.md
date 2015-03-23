# Earthquake
## Asra Nizami and Guillermo Vera

This is an earthquake visualization homework with historical data. Key points of the implementation: Tessellation of a sphere, conversion of polar to cartesian coordinates and earthquake representation.

### Design choices:
Earthquakes are represented by half spheres (which in reality are full spheres) placed on the geographic coordinate where an earthquake took place. The size of the sphere is proportional to 2<sup>m</sup>, where m is the magnitude of the earthquake. The exponential size attempts to imitate the logarithmic nature of the Richter Scale. 

The color of the earthquake spheres changes with time, changing from red [RGB: (1, 0, 0)] when it first appears to yellow [RGB: (1, 1, 0)] right after disappearing from the Earth. To display this we used the following formula for each channel in RGB:
```
ic = (1 - t) * c0 + t * c1;
```
Where `ic` is the interpolated channel, `c0` is the level of the channel of the first color, `c1` is the level of the channel of the second color, and `t` is in the interval `[0, 1]` and represents the time period between an earthquake and a year after it happened.

To compute `t` we used the following formula:
```
(currentTime - e.getDate().asSeconds()) / PLAYBACK_WINDOW
```
The current time subtracted by the earthquake's time returns the time between them and the division by the playback window (set to one year) normalizes it between 0 and 1.

### Earthquake adaptation to our coordinate system
The earthquake data contains the latitude and longitude of each earthquake in the standard GPS format: 
- latitude between 90˚ and -90˚ with 0˚ being at the equator.
- longitude between 180˚ and -180˚ with 0˚ being at the Greenwich (Prime) meridian.

Our system is different:
- latitude between 0˚ to 180˚ with 90˚ at the equator.
- longitude between 0˚ and 360˚ with 180˚ at the Greenwich meridian

To synchronize both systems, the latitude of the earthquakes was multiplied by `-1` and added `90`. The longitude was simply increased by `180`. This ensures that the earthquakes are being diplayed were they actually occurred.

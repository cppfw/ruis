# Manifest #

TODO: rewise this

Put the following under **`<manifest></manifest>`** tag:

```
<supports-screens
android:anyDensity="true"
android:largeScreens="true"
android:xlargeScreens="true"
/>
```

Otherwise, OpenGL viewport will be incorrect.
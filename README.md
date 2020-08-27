# DSTR_Project
Real-time mesh fracturing with dynamic impact locations. Developed for my Bachelor's Thesis.<br/>
<br/>

![Preview](/images/ezgif-2-fd182333765a.gif)<br/>

# Links
[YouTube](https://www.youtube.com/watch?v=aCtCb6J7cbE)<br/>

[Demo Application](https://github.com/FiskNi/DSTR_Project/releases/tag/v1.0)<br/>
[Voronoi Library](https://github.com/JCash/voronoi)<br/>

[Thesis](http://urn.kb.se/resolve?urn=urn:nbn:se:bth-20161)<br/>
[GitHub for game project](https://github.com/StevenCederrand/Night-of-the-Wizardlings)<br/>


# About

This implementation uses 2D Voronoi diagrams as fracture patterns which are clipped with the polygon where an impact occurs. There is no pre-fragmentation. To lift the result into 3D, each resulting diagram cell is extruded in one direction based on the depth of the original mesh.
The Voronoi diagrams themselves were implemented with the help of jc_voronoi (https://github.com/JCash/voronoi). 
More details can be found in the Thesis itelf, but overall it's a fairly simple implementation.

![Preview](/images/t_breach_200p.png)<br/>



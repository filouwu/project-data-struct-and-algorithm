# project-data-struct-and-algorithm
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Movhex - Optimal Route Computation</title>
</head>

<body>

<h1>Movhex – Optimal Route Computation</h1>

<p>
Final project for the <b>Algorithms and Data Structures course (2024-2025)</b>.
The program computes optimal routes for vehicles moving on a
<b>hexagonal grid map</b> representing a geographic area.
</p>

<div class="section">
<h2>Project Description</h2>

<p>
The planet surface is modeled as a <b>rectangular grid of hexagonal tiles</b>.
Each hexagon is identified by coordinates <code>(column, row)</code>.
</p>

<ul>
<li>Each hexagon connects to up to <b>6 neighbors</b></li>
<li>Each hexagon has a <b>ground exit cost</b></li>
<li>Each hexagon may contain up to <b>5 outgoing air routes</b></li>
</ul>

<p>
Movement cost depends on the type of connection:
</p>

<ul>
<li><b>Ground movement</b>: cost of the starting hexagon</li>
<li><b>Air route</b>: specific route traversal cost</li>
<li><b>Cost 0</b>: hexagon cannot be exited</li>
</ul>

</div>

<div class="section">

<h2>Program Interaction</h2>

<p>
The program reads commands from <b>standard input</b> and writes responses
to <b>standard output</b>.
</p>

</div>

<div class="section">

<h2>Commands</h2>

<h3>Map Initialization</h3>

<pre>
init &lt;columns&gt; &lt;rows&gt;
</pre>

<p>Initializes the map.</p>

<ul>
<li>All hexagons start with cost = 1</li>
<li>No air routes exist</li>
</ul>

<p>Output:</p>

<pre>
OK
</pre>

</div>

<div class="section">

<h3>Cost Update</h3>

<pre>
change_cost &lt;x&gt; &lt;y&gt; &lt;value&gt; &lt;radius&gt;
</pre>

<p>
Updates the cost of hexagons within a certain distance from the hexagon
<code>(x,y)</code>.
</p>

<p>Output:</p>

<pre>
OK
or
KO
</pre>

</div>

<div class="section">

<h3>Air Route Toggle</h3>

<pre>
toggle_air_route &lt;x1&gt; &lt;y1&gt; &lt;x2&gt; &lt;y2&gt;
</pre>

<p>
Adds an air route if it does not exist, otherwise removes it.
</p>

<ul>
<li>Maximum 5 outgoing air routes per hexagon</li>
<li>New route cost = average of existing routes and ground cost</li>
</ul>

<p>Output:</p>

<pre>
OK
or
KO
</pre>

</div>

<div class="section">

<h3>Travel Cost Query</h3>

<pre>
travel_cost &lt;xp&gt; &lt;yp&gt; &lt;xd&gt; &lt;yd&gt;
</pre>

<p>
Computes the <b>minimum cost</b> to reach the destination.
</p>

<ul>
<li>Destination hexagon exit cost is ignored</li>
<li>Using an air route ignores ground exit cost</li>
<li>If unreachable → result = -1</li>
</ul>

</div>

<div class="section">

<h2>Algorithmic Model</h2>

<p>
The map can be modeled as a <b>dynamic weighted graph</b>.
</p>

<ul>
<li><b>Nodes</b>: hexagons</li>
<li><b>Edges</b>: ground adjacency</li>
<li><b>Directed edges</b>: air routes</li>
</ul>

<p>
Shortest path queries are typically solved using
<b>Dijkstra's algorithm with a priority queue</b>.
</p>

</div>

<div class="section">

<h2>Complexity</h2>

<table>
<tr>
<th>Operation</th>
<th>Complexity</th>
</tr>

<tr>
<td>init</td>
<td>O(n × m)</td>
</tr>

<tr>
<td>change_cost</td>
<td>O(r²)</td>
</tr>

<tr>
<td>toggle_air_route</td>
<td>O(1)</td>
</tr>

<tr>
<td>travel_cost</td>
<td>O(E log V)</td>
</tr>

</table>

</div>

<div class="section">

<h2>Example Input</h2>

<pre>
init 100 100
change_cost 10 20 -10 5
travel_cost 0 0 20 0
toggle_air_route 0 0 20 0
travel_cost 0 0 20 0
</pre>

<h2>Example Output</h2>

<pre>
OK
OK
20
OK
1
</pre>

</div>

<div class="section">

<h2>Technologies</h2>

<ul>
<li>C / C++</li>
<li>Graph algorithms</li>
<li>Dijkstra shortest path</li>
</ul>

</div>

</body>
</html>

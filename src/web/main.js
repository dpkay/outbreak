var canv = document.getElementById('canvas');
var Module = {canvas : canv};

// -----------------------------------------------------------------------------
// Interface from C++ to JS (through WebAssembly).
//
// Keep in sync with viz.cc.
// -----------------------------------------------------------------------------
function ccToJs_reportSimulationStateJson(json) {
  let simulationState = JSON.parse(json);
  console.log(simulationState);
  console.log("Days: " + simulationState.hoursElapsed / 24);
}

//var svgWidth = 500;
//var svgHeight = 300;
//
//var svg = d3.select('svg')
//    .attr("width", svgWidth)
//    .attr("height", svgHeight)
//    .attr("class", "bar-chart");

 var margin = {top: 20, right: 20, bottom: 70, left: 40},
     width = 600 - margin.left - margin.right,
     height = 300 - margin.top - margin.bottom;

// set the ranges
var x = d3.scaleBand()
          .range([0, width])
          .padding(0.1);
var y = d3.scaleLinear()
          .range([height, 0]);

// var xAxis = d3.svg.axisBottom()
//     .scale(x)
//     .tickFormat(d3.time.format("%Y-%m"));
//
// var yAxis = d3.svg.axisLeft()
//     .scale(y)
//     .ticks(10);

 var svg = d3.select("svg")
               .attr("width", width + margin.left + margin.right)
               .attr("height", height + margin.top + margin.bottom)
               .attr("class", "bar-chart")
               .append("g")
               .attr("transform",
                     "translate(" + margin.left + "," + margin.top + ")");

 svg.append("g")
      .attr("transform", "translate(0," + height + ")")
      .call(d3.axisBottom(x));

  // add the y Axis
  svg.append("g")
      .call(d3.axisLeft(y));

var dataset = [80, 100, 56, 120, 180, 30, 40, 120, 160];

var barPadding = 5;
var barWidth = (width / dataset.length);

var barChart = svg.selectAll("rect")
    .data(dataset)
    .enter()
    .append("rect")
    .attr("y", function(d) {
        return height - d
    })
    .attr("height", function(d) {
        return d;
    })
    .attr("width", barWidth - barPadding)
    .attr("transform", function (d, i) {
         var translate = [barWidth * i, 0];

         return "translate("+ translate +")";
    });

 //
 // d3.csv("bar-data.csv", function(error, data) {
 //
 //     data.forEach(function(d) {
 //         d.date = parseDate(d.date);
 //         d.value = +d.value;
 //     });
 //
 //   x.domain(data.map(function(d) { return d.date; }));
 //   y.domain([0, d3.max(data, function(d) { return d.value; })]);
 //
 //   svg.append("g")
 //       .attr("class", "x axis")
 //       .attr("transform", "translate(0," + height + ")")
 //       .call(xAxis)
 //     .selectAll("text")
 //       .style("text-anchor", "end")
 //       .attr("dx", "-.8em")
 //       .attr("dy", "-.55em")
 //       .attr("transform", "rotate(-90)" );
 //
 //   svg.append("g")
 //       .attr("class", "y axis")
 //       .call(yAxis)
 //     .append("text")
 //       .attr("transform", "rotate(-90)")
 //       .attr("y", 6)
 //       .attr("dy", ".71em")
 //       .style("text-anchor", "end")
 //       .text("Value ($)");
 //
 //   svg.selectAll("bar")
 //       .data(data)
 //     .enter().append("rect")
 //       .style("fill", "steelblue")
 //       .attr("x", function(d) { return x(d.date); })
 //       .attr("width", x.rangeBand())
 //       .attr("y", function(d) { return y(d.value); })
 //       .attr("height", function(d) { return height - y(d.value); });
 //
 // });

// This files needs some work.

function expSortDown() {
	heatMap.sort( function(a,b) {return Number(b.intensity) - Number(a.intensity);});
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

function expSortUp() {
	heatMap.sort( function(a,b) {return Number(a.intensity) - Number(b.intensity);});
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

function expNormal() {
	heatMap = '';
	heatMap = JSON.parse(JSON.stringify(heatMapRaw));
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

function nonZero() {
	heatMap = heatMap.filter(function (d) { return d.intensity > 0; });
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}


function exp() {
		if(heatMap.length < 1) {
			return;
		}
//	heatMap = expSort(heatMap);
	//var colWidth = 15;
	var bodyWidth = d3.select("figure").node().getBoundingClientRect().width;
	//console.log(bodyWidth);
	var hGet = d3.select("figure").append("expfeaturechar").append("text").attr("font-family", "monospace").text('aWw');
        var heightChar = d3.select("text").node().getBoundingClientRect().height;
        var widthChar = d3.select("text").node().getBoundingClientRect().width;
	ratio = heightChar/widthChar;
	d3.select("expfeaturechar").remove();
	//ratio = 0.4444444444444444;//charWidth/charHeight;

	maxHeat = d3.max(heatMap, function (d) {return Number(d.intensity);});
	minHeat = d3.min(heatMap, function (d) {return Number(d.intensity);});
//	var obj = JSON.parse(heatMap);
//		console.log(obj);
	var loc = 0;
	var heatMapLength = heatMap.length;
	expKey(maxHeat, minHeat);
	while(1) {
		//just in case this loop is reached without an empty maxHeat
		if(heatMap.length < 1) {
			break;
		}
		var num = d3.max(heatMap.slice(loc,heatMapLength), function(d,i) { 
			if( d.name.length <= maxLength ) { 
				l = d.name.length;
			}
			else if ( maxLength == 0 ) {
				l = d.name.length;
			}
			else {
				l = maxLength;
			}
			if( (((i+.5)*colWidth) + l*ratio*.707*colWidth*.9) > bodyWidth ) {                  
		//		console.log((((i+.5)*colWidth) + colWidth*.9*l*ratio*.707) ); 
		//		console.log(ratio);
				//console.log(i);
				//console.log(colWidth);
				return 0;
			} 
			else {                 
		//		console.log((((i+.5)*colWidth) + colWidth*.9*l*ratio*.707) ); 
		//		console.log(ratio);
				//console.log(i);
				//console.log(colWidth);
				return i+1; 
			}
		});
//		console.log(num);
//		console.log(bodyWidth);
//		console.log(colWidth);
		subHeatMap = [];	
		/*for(i = 0; i < num; i++) {
			subHeatMap.push(heatMap[i]);
		}*/
	//	console.log(subHeatMap);
		if(loc+num >=heatMapLength-1){
			expSub(heatMap.slice(loc,loc + num+1),maxHeat,minHeat);
		}
		else {
			expSub(heatMap.slice(loc,loc + num),maxHeat,minHeat);
		}

		loc += num;
		if( loc >= heatMapLength) {	
			break;
		}
	}
}

function expKey(maxHeat, minHeat) {
	recSize = colWidth;
	if(colWidth < 15) {
		recSize = 15;
	}
	var rSH = recSize/2;
    var heatMapKey = [{"r": 255, "g": 0, "b": 0, "text": "min expression (" + minHeat + ") -"},
                      {"r": 0, "g": 0, "b": 0, "text": ""},
                      {"r": 0, "g": 255, "b": 0, "text": " - (" + maxHeat + ") max expression"}];
////////*****=======

	var keyContainer = d3.select("figure").append("expkeydom").append("svg")
	var keyTextContainer = keyContainer.append("g");
   
    var keyText = keyTextContainer.selectAll("text")
                              .data(heatMapKey)
                              .enter()
                              .append("text");
    var mapKeyText = keyText
                       .text( function (d) {return d.text;})
                       .attr("fontFamily", "monospace") //not working???
                       .attr("fontWeight", "bold") //not working for some reason
                       .attr("fill","black")
                       .each(function (d) {d.titleWidth = this.getBBox().width;})
                       .attr("y", recSize*.75);
                      
                       pushBack = heatMapKey[2].titleWidth;
                       pushForward = heatMapKey[0].titleWidth;
                      
                       keyText
                         .attr("x", function (d,i) {
                        if (i == 0) { return 0;}
                        if (i == 2) { return recSize*4 + pushForward; }
                      });

    var mapKey = keyContainer.append("g");
   
    var key = mapKey.selectAll("rect")
                    .data(heatMapKey)
                    .enter()
                    .append("rect");
                   
    var mapKeyConstruct = key
                       .attr("x", function (d,i) { return recSize*(i+.5) + pushForward;})
                       .attr("y", recSize*.25)
                       .attr("height", recSize*.5)
                       .attr("width", recSize)
                       .style("fill", function (d) {return d3.rgb(d.r,d.g,d.b);});
                  
	d3.select("svg")
		.attr("width", pushForward + (4*recSize) + pushBack)
		.attr("height", recSize);

                      /*  d3.select("svg")
                        .attr("transform", function (d) { return "translate(" + maxFeatureWidth + "," + maxTitleHeight + ")";})
                        .attr("width", divWidth)
                        .attr("height",parseFloat(3*recSize)+parseFloat(maxTitleHeight));*/ 
////////*****=======
	


}



function expSub(heatMap,maxHeat,minHeat) {
	minNorm = 0;
	if(minHeat < 0) {
		minNorm = -minHeat;
	}
		
        //var maxLength = 13;
	var graphHeight = Number(colWidth);
	if(col == "column") {
		graphHeight = 100;
	}
	var recSize =20;
	var rSH = recSize/2;

	var ratioChar = [{"char": "a"}];

	//maxHeat = d3.max(heatMap, function (d) {return Number(d.intensity);});
	hF = 511/maxHeat;
	numTiles = heatMap.length;

	//Make sure this works in multiple browsers
	var bodyWidth = d3.select("figure").node().getBoundingClientRect().width;
	var bodyHeight = d3.select("figure").node().getBoundingClientRect().height;
	//document.write(bodyWidth);


	var graphContainer = d3.select("figure").append("expfeaturedom").append("svg")
		.attr("width",bodyWidth)
		.attr("height",graphHeight);

	div = d3.select("figure").append("div")
		.attr("class","tooltip")
		.style("opacity", 0);

	var fontRatio = graphContainer.append("g")
		.selectAll("text")
		.data(ratioChar)
		.enter()
		.append("text")
		.attr("x", 0)
		.attr("y", 0)
		.text( function (d) {return d.char;})
		.each(function (d) { d.charHeight = this.getBBox().height;})
		.each(function (d) { d.charWidth = this.getBBox().width;});

	charHeight = d3.max(ratioChar, function (d) {return d.charHeight;});
	charWidth = d3.max(ratioChar, function (d) {return d.charWidth;});

	ratio = charWidth/charHeight;
	t = 1000;
	var n = d3.min(heatMap, function (d,i) { if(d.name.length <= maxLength || maxLength == 0) {return t/((i+.5) + (d.name.length*ratio*.707))} else {return t/((i+.5) + (maxLength*ratio*.707)) }});

	bodyWidth1 = ((n*heatMap.length)/t)*bodyWidth;
	//document.write((n*heatMap.length)/t + "\n");
//	var colWidth = (n/1000)*bodyWidth;//bodyWidth1/heatMap.length;

	if(colWidth > 25) {
//		colWidth = 25;
	}
//	colWidth = 15;
////////////////////

	var graphBackground = graphContainer.append("g")
		.selectAll("rect")
		.data(heatMap)
		.enter()
		.append("a")
		.attr("xlink:href", function(d) { return "/biomaterial/" + d.name})
		.append("rect")
		.each(function (d) { d.intensityLabel = d.intensity;})
		.each(function (d) { d.intensityLabel = Number(d.intensity) + Number(minNorm);});

	var graphBackgroundConstruct = graphBackground
		.attr("transform", function (d,i) {return "translate(" + i*colWidth + "," + 0 + ")";})
		.attr("height", graphHeight)
		.attr("width", colWidth)
		.on("mouseover", function (d,i) {
				div.transition()
				.duration(200)
				.style("opacity", .9)
				div .html(d.name + ': ' + d.intensityLabel)
				.style("left", (d3.event.pageX) + "px")
				.style("border-color", function() {
					if(d.intensity == 0) {
						return d3.rgb(204,204,204);
					} 
					if ((d.intensity*hF) <= 255) {
						return d3.rgb(255-(d.intensity*hF),0,0);
					} 
					else {
						return d3.rgb(0,(d.intensity*hF)-255,0);
					}
				})
				.style("top", (d3.event.pageY - 28) + "px");
		})
	.on("mouseout", function (d,i) {
			div.transition()
			.duration(500)
			.style("opacity",0);
			})
	//.on("click", function(d) {window.location = window.location.href + "/biomaterial/"+d.name;})
	.style("fill", "white");


	//document.write("made it to this line");


///////////////////
	var graphGroup = graphContainer.append("g")
		.selectAll("rect")
		.data(heatMap)
		.enter()
		.append("a")
		.attr("xlink:href", function(d) { return "/biomaterial/" + d.name})
		.append("rect");

	var graphConstruct = graphGroup
		.attr("transform", function (d,i) { 
			if(col == "column") {
				return "translate(" + i*colWidth + "," + (graphHeight - ((graphHeight*d.intensity)/maxHeat)) + ")";
			} 
			return "translate(" + i*colWidth + "," + (graphHeight - colWidth) + ")";
		})
		.attr("height", function(d) {
			if(col == "column") { 
				return (graphHeight*d.intensity)/maxHeat;
			} 
			return colWidth;
		})
		.attr("width", colWidth)
		.on("mouseover", function (d,i) {
				div.transition()
				.duration(200)
				.style("opacity", .9)
				div .html(d.name + ': ' + d.intensityLabel)
				.style("left", (d3.event.pageX) + "px")
				.style("border-color", function() {
					if ((d.intensity*hF) <= 255) {
						return d3.rgb(255-(d.intensity*hF),0,0);
					} 
					else {
						return d3.rgb(0,(d.intensity*hF)-255,0);
					}
				})
				.style("top", (d3.event.pageY - 28) + "px");
		})
	.on("mouseout", function (d,i) {
			div.transition()
			.duration(500)
			.style("opacity",0);
			})
	//.on("click", function(d) {window.location = window.location.href + "/biomaterial/"+d.name;})
	.style("fill", function (d) { if ((d.intensity*hF) <= 255) {
					return d3.rgb(255-(d.intensity*hF),0,0);
				} 
				else {
					return d3.rgb(0,(d.intensity*hF)-255,0);
				}
		});

//******************

	var graphGroup = graphContainer.append("g")
		.selectAll("rect")
		.data(heatMap)
		.enter()
		.append("a")
		.append("rect");

	var graphConstruct = graphGroup
		.attr("transform", function (d,i) {return "translate(" + i*colWidth + "," + (graphHeight - ((graphHeight*d.intensity)/maxHeat)) + ")";})
		.attr("height", function(d) {if(d.intensity > 0) {return 0} return 1})
		.attr("width", colWidth)
	.style("fill", d3.rgb(0,0,0));




//*****************



//	var showLabels = 1;

	if(showLabels == 1) {
		maxLength = 1;
	}


	//document.write("made it to this line");

	var graphLables = graphContainer.append("g")
		.selectAll("text")
		.data(heatMap)
		.enter()
		.append("text")
		.attr("x", function (d,i) {return (i*colWidth+(.5*colWidth)-(.25*colWidth));})
		.attr("y", graphHeight + (colWidth/2))
		.text( function (d) { 
			if(d.name.length <= maxLength || maxLength == 0) {
				return d.name;
			} 
			else { 
				if (showLabels !=1) {
					return d.name.substring(0,maxLength - 3) + '...'; 
				}
				return d.name.substring(0,maxLength - 3); 
				
			}
		})
		.attr("font-family", "monospace")
		.attr("fontWeight", "bold") //not working for some reason
		.style("font-size", colWidth*0.707106781*.9 + "px")
		.attr("fill","black")
		.attr("transform", function (d,i) { 
			return "rotate(45 " + (colWidth*i+(.5*colWidth)-(.25*colWidth)) + "," + (graphHeight+(colWidth/2)) + ")";
		})
		.each(function (d) { d.titleHeight = 0.707*this.getBBox().width;})
		.each(function (d,i) { d.titleend = (colWidth*i+(.5*colWidth)-(.25*colWidth))+(0.707*this.getBBox().width);});


	var maxTitleHeight = d3.max(heatMap, function (d) {return d.titleHeight;});
	var divWidth = d3.max(heatMap, function (d) {return d.titleend;});

	//d3.selectAll("svg")
	graphContainer
		//      .attr("transform", function (d) { return "translate(" + maxFeatureWidth + "," + maxTitleHeight + ")";})
	//	.attr("width", divWidth+(colWidth*0.707106781))
		.attr("height",graphHeight + maxTitleHeight+(colWidth*0.707106781));
//	}


/*	else {


	graphContainer
		//      .attr("transform", function (d) { return "translate(" + maxFeatureWidth + "," + maxTitleHeight + ")";})
		.attr("height",graphHeight);




	}*/

	d3.select(window).on('resize', resize);


	function resize() {


	}

	//document.write(bodyWidth + "\n");
	//document.write(bodyWidth1);
};

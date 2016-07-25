/**
 * This is the default function that is called when the page is loaded. 
 * It is also called when the reset link is clicked. It will order biomaterials
 * using alphanumeric ordering.
 */
function expNormal() {
	heatMap = '';
	heatMap = JSON.parse(JSON.stringify(heatMapRaw));
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

/**
 * This function is called to change the graph type to chart.
 */
function expChart() {
	if (col == "column") {
		col = "tile";
	}
	else if (col == "tile") {
		col = "column";
	}
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

/**
 * This will arrange the biomaterials by value from greatest to least.
 */
function expSortDown() {
	heatMap.sort( function(a,b) {return Number(b.intensity) - Number(a.intensity);});
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

/**
 * This will arrange the biomaterials by value from least to greatest.
 */
function expSortUp() {
	heatMap.sort( function(a,b) {return Number(a.intensity) - Number(b.intensity);});
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

/**
 * This function will remove biomaterials that have a value of 0.
 */
function nonZero() {
	heatMap = heatMap.filter(function (d) { return d.intensity > 0; });
	d3.selectAll("expfeaturedom").remove();
	d3.selectAll("expkeydom").remove();
	exp();
}

/**
 * This function will divide the biomaterials up into different lines based
 * on the column width set by the user. This function will call the expKey 
 * function to build the table key. It will call the expSub function for each
 * line of biomaterials.
 */
function exp() {
	/* Only print out heatmap if there is at least one value. */
	if(heatMap.length < 1) {
		return;
	}
	var bodyWidth = d3.select("figure").node().getBoundingClientRect().width;

	/* Find the height to width ratio of a monospace character. */
	var hGet = d3.select("figure").append("expfeaturechar").append("text").attr("font-family", "monospace").text('a');
        var heightChar = d3.select("text").node().getBoundingClientRect().height;
        var widthChar = d3.select("text").node().getBoundingClientRect().width;
	ratio = heightChar/widthChar;
	/* Remove the character. */
	d3.select("expfeaturechar").remove();
	//ratio = 0.4444444444444444;//charWidth/charHeight;

	/* The max and min heat will be used in creating the figure key. */
	maxHeat = d3.max(heatMap, function (d) {return Number(d.intensity);});
	minHeat = d3.min(heatMap, function (d) {return Number(d.intensity);});
	var loc = 0;
	var heatMapLength = heatMap.length;
	
	/* Create the figure key. */
	expKey(maxHeat, minHeat);

	/* Draw the heat maps line by line. */	
	while(1) {
		/* Grab the biomaterials that have not yet been drawn. */
		var num = d3.max(heatMap.slice(loc,heatMapLength), function(d,i) { 

			/* Make sure the lenth of the biomaterial name does not exceed
                        the maxLength set by the user. */
			if( d.name.length <= maxLength ) { 
				l = d.name.length;
			}
	
			/* If the maxLength variable is set to 0, there is not limit
                        on the biomaterial name length. */
			else if ( maxLength == 0 ) {
				l = d.name.length;
			}
			
			/* Truncate the length of the biomaterial name to maxLength. */
			else {
				l = maxLength;
			}
	
			/* Find the max number of biomaterials that column width and biomaterial name allow. */	
			if( (((i+.5)*colWidth) + l*ratio*.707*colWidth*.9) > bodyWidth ) {                  
				return 0;
			} 
			else {                 
				return i+1; 
			}
		});
	
		/* Cut up the biomateial json variable to draw on each line. */	
		subHeatMap = [];	
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

/*
 * This function creates a key for the figure. 
 */
function expKey(maxHeat, minHeat) {
	recSize = colWidth;

	/* Set a the minimum column width so that the figure does not become to hard to read. */
	if(colWidth < 15) {
		recSize = 15;
	}
	var rSH = recSize/2;

	/* Set the colors of the key. The colors are green, black, and red. */
	var heatMapKey = [{"r": 255, "g": 0, "b": 0, "text": "min expression (" + minHeat + ") -"},
		{"r": 0, "g": 0, "b": 0, "text": ""},
		{"r": 0, "g": 255, "b": 0, "text": " - (" + maxHeat + ") max expression"}];

	/* Configure the key text. */
	var keyContainer = d3.select("figure").append("expkeydom").append("svg")
	var keyTextContainer = keyContainer.append("g");
	var keyText = keyTextContainer.selectAll("text")
		.data(heatMapKey)
		.enter()
		.append("text")
		.text( function (d) {return d.text;})
		.attr("fontFamily", "monospace") //not working???
		.attr("fontWeight", "bold") //not working for some reason
		.attr("fill","black")
		.each(function (d) {d.titleWidth = this.getBBox().width;})
		.attr("y", recSize*.75);
                      
	pushBack = heatMapKey[2].titleWidth;
	pushForward = heatMapKey[0].titleWidth;

	/* Set the location of the key text. */                      
	keyText.attr("x", function (d,i) {
		if (i == 0) { return 0;}
		if (i == 2) { return recSize*4 + pushForward; }
	});

	/* Draw the tiles for the key. */
	var mapKey = keyContainer.append("g");
	var key = mapKey.selectAll("rect")
		.data(heatMapKey)
		.enter()
		.append("rect")
		.attr("x", function (d,i) { return recSize*(i+.5) + pushForward;})
		.attr("y", recSize*.25)
		.attr("height", recSize*.5)
		.attr("width", recSize)
		.style("fill", function (d) {return d3.rgb(d.r,d.g,d.b);});

	/* Set dimensions of the svg. */
	d3.select("svg")
		.attr("width", pushForward + (4*recSize) + pushBack)
		.attr("height", recSize);

}

/*
 * The function draws an single line of biomaterials.
 */
function expSub(heatMap,maxHeat,minHeat) {

	/* Normalize to minimum value to 0 incase of negative expression values. */
	minNorm = 0;
	if(minHeat < 0) {
		minNorm = -minHeat;
	}

	/* Graph height is hardcoded to 100. */		
	var graphHeight = Number(colWidth);
	if(col == "column") {
		graphHeight = 100;
	}

	hF = 511/maxHeat;
	numTiles = heatMap.length;

	/* Get the size of the figure. */
	var bodyWidth = d3.select("figure").node().getBoundingClientRect().width;
	var bodyHeight = d3.select("figure").node().getBoundingClientRect().height;

	var graphContainer = d3.select("figure").append("expfeaturedom").append("svg")
		.attr("width",bodyWidth)
		.attr("height",graphHeight);

	div = d3.select("body").append("div")
		.attr("class","tooltip")
		.style("opacity", 0);

	var ratioChar = [{"char": "a"}];
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
		.attr("xlink:href", function(d) { if(d.node) {return "/biomaterial/" + d.name} return null;})
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
                                div .html(function() {mrkup = '<b>' + d.name + '</b>: <b>' + d.intensityLabel + '</b>';
                                        if (d.treatment != '') {
                                                mrkup += '<br><b>Treatment</b>: ' + d.treatment;      
                                        }
                                        if (d.tissue != '') {
                                                mrkup += '<br><b>Tissue</b>: ' + d.tissue;
                                        }
                                        if (d.description != '') {
                                                mrkup += '<br><b>Description</b>: ' + d.description;
                                        }
                                        return mrkup;
                                })
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
	.style("fill", "transparent");


	//document.write("made it to this line");


///////////////////
	var graphGroup = graphContainer.append("g")
		.selectAll("rect")
		.data(heatMap)
		.enter()
		.append("a")
		.attr("xlink:href", function(d) { if(d.node) {return "/biomaterial/" + d.name} return null;})
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
				.style("text-align", "left")
                                div .html(function() {mrkup = '<b>' + d.name + '</b>: <b>' + d.intensityLabel + '</b>';
                                        if (d.treatment != '') {
                                                mrkup += '<br><b>Treatment</b>: ' + d.treatment;      
                                        }
                                        if (d.tissue != '') {
                                                mrkup += '<br><b>Tissue</b>: ' + d.tissue;
                                        }
                                        if (d.description != '') {
                                                mrkup += '<br><b>Description</b>: ' + d.description;
                                        }
                                        return mrkup;
                                })
				.style("max-width", 200 + "px")
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
	.style("fill", function (d) { if ((d.intensity*hF) <= 255) {
					return d3.rgb(255-(d.intensity*hF),0,0);
				} 
				else {
					return d3.rgb(0,(d.intensity*hF)-255,0);
				}
		});

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

	if(showLabels == 1) {
		maxLength = 1;
	}

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

	graphContainer.attr("height",graphHeight + maxTitleHeight+(colWidth*0.707106781));

//	d3.select(window).on('resize', resize);
};

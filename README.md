[![DOI](https://zenodo.org/badge/49081816.svg)](https://zenodo.org/badge/latestdoi/49081816)


# Tripal\_analysis\_expression

This is an extension module for the Tripal project. 

Please note this module requires **Tripal 3** or greater.  The [Tripal 2 functional module is available for download](https://github.com/tripal/tripal_analysis_expression/releases/tag/1.0.2) but is no longer supported.

# Tripal Analysis: Expression

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Module Features](#features)
4. [Loading Biosamples](#loading-biosamples)
5. [Loading Expression Data](#loading-expression-data)
6. [Viewing Data](#viewing-and-downloading-data)
7. [Administrative Pages](#administrative-pages)
8. [Protocols](#protocols)
9. [Example Files](#example-files)

# Introduction 
Tripal Analysis: Expression is a [Drupal](https://www.drupal.org/) module built to extend the functionality of the [Tripal](http://tripal.info/) toolset.
The purpose of the module is to visually represent gene expression for Tripal features. This module requires the following Tripal modules:

1. Tripal
2. Tripal Chado
3. Tripal Biomaterial (available [here](https://github.com/tripal/tripal_biomaterial))


# Installation
1. Click on the green "Clone or download" button on the top right corner of this page to obtain the web URL. Download this module by running ```git clone <URL> ``` on command line. 
2. Place the cloned module folder "tripal_analysis_expression" inside your /sites/all/modules. Then enable the module by running ```drush en tripal_analysis_expression``` (for more instructions, read the [Drupal documentation page](https://www.drupal.org/node/120641)).

# Features
* Provides data loaders for expression data
* Controlled Vocabulary tools for biosamples
* Visualization for expression data for individual features
* Heatmap tool to visualize multiple features

### Data Loaders
This module provides an expression loader that can load expression data in column or matrix format.

### Expression Display
Once expression data is loaded, a display field will be shown on each feature page that has corresponding biosamples and expression values.

### Heatmap
This module provides a search and results block to search for and select features to display in a heatmap.


## Tripal 3 entities and fields

The following bundles are defined by `Tripal Protocol`

* Protocol
* Arraydesign

The following fields are defined by `Tripal Protocol`, `Tripal Biomaterial`, and `Tripal Analysis Expression`

* Expression Data
    - Analysis
    - Feature

* Biosample Browser
    - Analysis


# Loading Biosamples
The documentation for loading biosamples can be found [here](https://github.com/tripal/tripal_biomaterial).
    
# Loading Expression Data
The steps for loading expression data are as follows (detailed instructions can be found further below):

1. Obtain expression data. Click [here to read about the file formats accepted for expression data](#data-loader). 
2. Add the organism associated with the expression data if it hasn't been added. 
3. Upload all features in the expression data to the Chado database. To bulk upload features, go to **Tripal->Data Loaders->Chado FASTA Loader** and upload a FASTA file (click here to see an example of [fasta file of transcriptome sequences](http://www.hardwoodgenomics.org/sites/default/files/sequences/sugarMaple022416/Acer_saccharum_022416_transcripts.fasta)). Or upload one feature at a time via **content-> Tripal Content -> Add content**, and select the relevant entity type (such as mRNA).
4. Load the espression data.  This is also the step where you can add experimental design details.

### Creating the Analysis
Before loading data, describe the experimental setup used to collect the data. As an administrator or a user with permission to create content, navigate to content -> tripal content -> Analysis.

**Note that program name, program version, and source name must be unique as a whole for analysis to be inserted correctly** (click [here](http://gmod.org/wiki/Chado_Companalysis_Module) to read more about the data structure for analysis).

#### Analysis Fields:

* **Analysis Name (required)**
* **Analysis Description**
* **Program, Pipeline Name or Method Name (required, part of unique constraint)**
* **Program, Pipeline or Method version (required, part of unique constraint)**
* **Algorithm**
* **Data Source Name (required, part of unique constraint)**
* **Source Version**
* **Source URI**
* **Time Executed (required)**

#### Expression Data Loader

The Chado Expression Data Loader provide a way for the user to load expression data associated with the experiment. The loader can load data from two types of formats, matrix and column. The matrix format expects a row of data containing biosample names. The first column should be unique feature names. Features must already be loaded into the database. Biosamples will be added if not present. Expression values will map to a biosample library in the column and a feature in the row. Only one matrix file may be loaded at a time. The column format expects the first column to contain features and the second column to be expression values. 

For an example column file, click [here](example_files/exampleExpressionData.rpkm). For an example matrix file, click [here](example_files/exampleMatrix.tsv).

The biosample name will be taken as the name of the file minus the file extension. Features must already be loaded into the database. Biosamples will be added if not present. Multiple column format files may be loaded at the same time by uploading multiple files or, if providing a server path, if all files are in the same folder with the same file extensions. Either format may have header or footer information. Regex can be used in the form to only record data after the header and before the footer.  The data loader fields are the following:

* File Upload - You may upload a file using the loader, or provide a path on the server.  The path may also be set to a directory, in which case all column files with the "File Type Suffix" specified above will be loaded. When loading multiple files, a file suffix (extension) must be specified. 

* **Analysis** - The analysis to associate the expression data with.
* **Organism (required)** - The organism.
* **Source File Type** - This can be either "Column Format" or "Matrix Format".
* **Name Match Type** - Will the data be associated with feature names or unique names?
* **File Type Suffix** - The suffix of the files to load. This is used to submit multiple column format files in the same directory. A suffix is not required for a matrix file.
* **Regex for Start of Data** - If the expression file has a header, use this field to capture the line that occurs before the start of expression data. This line of text and any text preceding this line will be ignored. 
* **Regex for End of Data** - If the expression file has a footer, use this field to capture teh line that occurs after the end of expression data. This line of text and all text following will be ignored.

#### Experimental Design Fields
The "Experimental Design" fields allow a complete description of the experimental design by implementing the [Chado MAGE design schema](http://gmod.org/wiki/Chado_Mage_Module).  The Chado MAGE module uses the arraydesign, assay, quantification, and acquisition tables to describe an experiment. The Tripal Analysis Expression creates generic instances of all these for you.
 
* **Array Design** - This is only applicable for microarray expression data. This may be left blank for experiments that do not utilize an array (ie next generation sequencing). 
* **Units** - The units associated with the loaded values, such as FPKM.  You may also update the units of your experiments using the **Quantification Units** admin page.

>![file information portion of expression loader](example_files/doc_images/expression_loader_file_info.png)
>![experimental design portion of expression loader](example_files/doc_images/expression_loader_experimental_design.png)
> The expression loader accepts parameters describing your file, as well as some experimental design parameters.

# Viewing and Downloading Data

Loaded expression data can be viewed and downloaded by users in three places.  **Feature pages** will gain access to the Expression field (`data__gene_expression_data`).  You can configure the appearance of this field by navigating to `Structure -> Tripal Content Type -> [Feature type (ie, mRNA)]`.  If the expression field is not listed, press the Check for New Fields button in the upper left.  Once the field is attached, navigate to the Manage Display tab, enable the field display, and place it to your liking. 


>![Creating and expression tripal pane for the expression field](example_files/doc_images/expression_pane.png)
>In this example, we have placed the Expression field in an a Tripal pane all of its own.

### Downloading data

Data downloads are provided for individual features, analyses, and for feature sets selected in the heatmap.  For data downloading to be functionaly, you must *populate the materialized views associated with this module*.  This can be done by navigating to Tripal -> Data Storage -> Chado -> Materialized Views.  Press the populate link for the **expression_feature** and *expression_feature_all** materialized views and run the submitted job.  Materialized views must be manually repopulated when you add new data.

### Using the Feature Expression Data field

The expression field allows users to view all expression data available for a feature.  Because a database might have multiple experiments involving a single feature, data is first organized by *Analysis*.  Users can select analyses using the "Select an Expression Analysis" box which lists all analyses with expression data available.  The plot can be further customized based on the biosample properties.  The "Select a property to group and sort biosamples" select box will allow users to pick a property to organize samples along the X axis.  Users may select *Sample Name* to elect not to group samples by property.  Values may be colored by their expression value (default), or by selecting a different property in the "Select a property to color biosamples " box.  

>![The feature expression data field](example_files/doc_images/Expression_grouped_and_colored.png)
> The Feature Expression Data field allows users to plot expression data according to biosample properties.

Once plotting parameters are set, users can click and drag to re-arrange both the legend and the individual groups.  The "Only Non-Zero Values" button removes samples with expression values of 0, tidying the plot.

Data can be downloaded in matrix format by pressing the "Download expression dataset for this feature" link.

### Using the Analysis Biosample Browser and Expression Data field

As with fields attached to feature, you must add the new Analysis fields and configure their display by navigating to `Structure -> Tripal Content Type -> Analysis`.

The Biomaterial browser will list biosamples and their properties.  The Expression Data field will not visualize expression data, but allow the user to download all expression data associated with this analysis in matrix form (rows: features, columns: biosamples).

# Searching features and displaying expression data in a heatmap
This module creates two blocks: one for features input and the other displaying a heatmap for the input features.

### Turn On blocks
Go to **Structure->blocks** and find these two blocks: ***tripal_analysis_expression features form for heatmap*** and ***tripal_elasticsearch block for search form: blast_merged_transcripts***. Configure these two blocks to display at specific region and page(s).  The ***tripal_analysis_expression features form for heatmap*** will display a form that allow you to input  feature IDs.

![feature-input-form](https://cloud.githubusercontent.com/assets/1262709/25756810/22149cf4-3196-11e7-8f22-089f316297a0.png)

After you enter feature IDs, you click the "Display Expression Heatmap" button to generate a heatmap for the features. 

# Administrative Pages

### Heatmap settings

The Heatmap settings administrative page allows you to configure the heatmap search form.  Here you can configure the placeholder text that appears, as well as the example feature ID's that the user can populate with the "try an example" button.  There is also a checkbox which enables elasticsearch integration.  Elasticsearch integration requires the `tripal_elasticsearch` module and a configured elasticsearch instance.  See the Tripal Elasticsearch module for more details and instructions.

### Quantification Units Administrative page

The units associated with your expression data are stored in Chado associated with the **quantification**.  Units can be stored even if you did not specify a quantification (a generic quantification was used in this case).  

You can use the quantification units administrative page to add or edit units on your quantification by Navigating to Tripal -> Extensions -> Protocol -> Quantification Units.  All quantification instances appear in the table at the bottom of the admin page. Click 'Edit' to change the units for an individual quantification. 

You can also assign quantification units to **all unitless quantifications** using the Assign Units box.




# Protocols

Acquisition, Quantification, Array Design, and Assays all utilize protocols to describe them.  Think of protocols as the **experimental design**, and Acquisitions, Quantifications, Array Designs, and Assays as the experiment following that experimental design.

Note that these content types are provided by **Tripal Core**.

There is currently no support for inputting, or displaying, acquisitions, quantifications, or assays.  The Expression module creates generic instances of these entities.

**Protocol Descripton** - The protocol content types can be created by navigating to **Add Tripal Content->Protocol**. A protocol can be used to add extra detail to an experimental design. A protocol can be used to describe the assay, acquisition, and quantification steps of the experiment design. A protocol can also be used to further describe the array design content type. The fields of a protocol are:
* **Protocol Name (must be unique - required)**
* **Protocol Link (Required)** - A web address to a page that describes the protocol.
* **Protocol Description** - A description of the protocol.
* **Hardware Description** - A description of the wardware used in the protocol.
* **Software Description** - A description of the software used in the protocol.
* **Protocol Type (required)** - The protocol type can acquisition, array design, assay, or quantification. The user can also create new protocol types by inserting new CVterms into the protocol type CV.
* **Publication** - A publication that describes the protocol.



# Example Files

### Biomaterial Loader
1. Flat files: [CSV file](example_files/exampleCSV.csv), [TSV file](example_files/exampleTSV.tsv)
2. [XML file](example_files/sm125.xml)

### Expression Data Loader
1. [Column file](example_files/exampleExpressionData.rpkm)
2. [Matrix file](example_files/exampleMatrix.tsv)

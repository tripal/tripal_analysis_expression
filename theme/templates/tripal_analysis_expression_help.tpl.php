  <h3> Analysis: Expression Content Type Description:</h3>
  <p> The content type <b>Analysis: Expression</b> is one of the content types specified by the 
    Tripal Expression module. The Tripal Expression module is built on top of the chado
    MAGE module. The Analysis: Expression content type is built on top of the Tripal Analysis 
    module and is used to detail the design of a microarray or RNA-seq expression experiment.</p> 

  <ul><li>For more information on the chado analysis table see the Chado Mage module
    <a href="http://gmod.org/wiki/Chado_Mage_Module">GMOD wiki page</a>.</li>
     <li>For more informate on the MAGE schema see the FGED 
    <a href="http://fged.org/projects/mage-tab/">MAGE-TAB page</a>.</li>
  </ul>

  <h3>Setup Instructions:</h3>
  <p>After installation of the Tripal Expression module, the following tasks should be performed.</p>
  <ul>
    <li><p><b>Set Permissions</b>: By default only the site administrator account has access to 
     create, edit, delete or administer array designs. Navigate to the <?php print 
     l('permissions page', 'admin/people/permissions')?> and set the permissions under the 'Tripal Expression' 
     section as appropriate for your site. For a simple setup, allow anonymous users access to 
     view content and create a special role for creating, editing and other administrative tasks.</p></li>

    <li><p><b>Create an Analysis: Expression</b>: Analysis: Expression pages can be created in two ways:</p>
      <ol>
        <li><p><b>Sync Analysis: Expression</b>: If your array design has been pre-loaded into Chado then you 
          need to sync the array design. This process is what creates the pages for viewing online. Not all 
          array designs need to be synced, only those that you want to show on the site. Use the 
          <a href="<?php print url('admin/tripal/extension/tripal_analysis_expression/content_types/analysis_expression/sync') ?>">Array design syncing page</a>
          to sync array designs. </p></li>
          <li><p><b>Manually Add An Analysis: Expression</b>: If your Analysis: Expressin is not already 
            present in the Chado database you can create an array design using the 
            <a href="<?php print url('node/add/chado-analysis-expression') ?>">Create Array Design page</a>.
          Once saved, the Analysis: Expression will be present in Chado and also "synced".</p>
      </ol>
    </li>
  </ul>

  <h3>Features of this Module:</h3>
  <p>Aside from Analysis: Expression page setup (as described in the Setup section above),
     The Tripal Expression module also provides the following functionality</p>
  <ul>
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/analysis-expression') ?> is provided for 
    finding Analysis: Expressions. This tool relies on Drupal Views. 
    <a href="https://www.drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool. Look for it in the navigation menu under the item "Search Data". </p></li>
  </ul>   
 
 


  <h3> Array Design Content Type Description:</h3>
  <p> The content type <b>Array Design</b> is one of the content types specified by the 
    Tripal Expression module. The Tripal Expression module is built on top of the chado
    MAGE module. The array design content type is used to detail the design of an array 
    used in a microarray experiment. If your expression data does not use an array design
    (e.g., next generation sequencing), you may specify a null array design for your 
    expression analysis.</p> 

  <ul><li>For more information on the chado arraydesign table see the Chado Mage module
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

    <li><p><b>Create an Array Design</b>: Array Design pages can be created in two ways:</p>
      <ol>
        <li><p><b>Sync Array Designs</b>: If your array design has been pre-loaded into Chado then you need to sync the array design.
          This process is what creates the pages for viewing online. Not all array designs need to be synced, only those
          that you want to show on the site. Use the 
          <a href="<?php print url('admin/tripal/extension/tripal_analysis_expression/content_types/arraydesign/sync') ?>">Array design syncing page</a>
          to sync array designs. </p></li>
          <li><p><b>Manually Add An Array Design</b>: If your array design is not already present in the Chado database
          you can create an array design using the <a href="<?php print url('node/add/chado-arraydesign') ?>">Create Array Design page</a>.
          Once saved, the array design will be present in Chado and also "synced".</p>
      </ol>
    </li>
  </ul>

  <h3>Features of this Module:</h3>
  <p>Aside from array design page setup (as described in the Setup section above),
     The Tripal Expression module also provides the following functionality</p>
  <ul>
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/arraydesign') ?> is provided for 
    finding array designs. This tool relies on Drupal Views. <a href="https://www.drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool. Look for it in the navigation menu under the item "Search Data". </p></li>
  </ul>   
 
 


  <h3> Module Decsription:</h3>
  <p> The content type <b>Biomaterial</b> is one of the content types specified by the 
    Tripal Expression module. The Tripal Expression module is built on top of the chado
    MAGE module. The biomaterial content type is used to detail a biomaterial. A biomaterial 
    record is required for each library in the expression matrix file.</p> 

  <ul><li>For more information on the chado biomaterial table see the Chado Mage module
    <a href="http://gmod.org/wiki/Chado_Mage_Module">GMOD wiki page</a>.</li>
     <li>For more informate on the MAGE schema see the FGED 
    <a href="http://fged.org/projects/mage-tab/">MAGE-TAB page</a>.</li>
  </ul>

  <h3>Setup Instructions:</h3>
  <p>After installation of the Tripal Expression module, the following tasks should be performed.</p>
  <ul>
    <li><p><b>Set Permissions</b>: By default only the site administrator account has access to 
     create, edit, delete or administer biomaterials. Navigate to the <?php print 
     l('permissions page', 'admin/people/permissions')?> and set the permissions under the 'Tripal Expression' 
     section as appropriate for your site. For a simple setup, allow anonymous users access to 
     view content and create a special role for creating, editing and other administrative tasks.</p></li>

    <li><p><b>Create a Biomaterial</b>: Biomaterial pages can be created in several  ways:  *** need to add links to the bulk and/or custom loaders</p>
      <ol>
        <li><p><b>Sync Biomaterials</b>: If your biomaterial has been pre-loaded into Chado then you need to sync the biomaterial.
          This process is what creates the pages for viewing online. Not all biomaterials need to be synced, only those
          that you want to show on the site. Use the 
          <a href="<?php print url('admin/tripal/extension/tripal_analysis_expression/biomaterial/sync') ?>">Biomaterial syncing page</a>
          to sync array designs. </p></li>
          <li><p><b>Manually Add A Biomaterial</b>: If your biomaterial is not already present in the Chado database
          you can create a biomaterial using the <a href="<?php print url('node/add/chado-biomaterial') ?>">Create Biomaterial page</a>.
          Once saved, the biomaterial will be present in Chado and also "synced".</p>
      </ol>
    </li>
  </ul>

  <h3>Features of this Module:</h3>
  <p>Aside from biomaterial page setup (as described in the Setup section above),
     The Tripal Expression module also provides the following functionality</p>
  <ul>
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/biomaterial') ?> is provided for 
    finding biomaterials. This tool relies on Drupal Views. <a href="https://www.drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool. Look for it in the navigation menu under the item "Search Data". </p></li>
  </ul>   
 
 


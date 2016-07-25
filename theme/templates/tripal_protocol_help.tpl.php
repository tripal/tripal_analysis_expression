  <h3> Protocol Content Type Description:</h3>
  <p> The content type <b>Protocol</b> is one of the content types specified by the 
    Tripal Expression module. The Tripal Expression module is built on top of the chado
    MAGE module. The array design content type is used to detail a protocol. A protocol 
    is used by the following tables: acquisition, arraydesign, assay, quantification
    and treatment. If you do not have a protocol, you may use an null protocol.</p> 

  <ul><li>For more information on the chado protocol table see the Chado Mage module
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

    <li><p><b>Create a Protocol</b>: Protocol pages can be created in two ways:</p>
      <ol>
        <li><p><b>Sync Protocols</b>: If your protocol has been pre-loaded into Chado then you need to sync the protocol.
          This process is what creates the pages for viewing online. Not all protocols need to be synced, only those
          that you want to show on the site. Use the 
          <a href="<?php print url('admin/tripal/extension/tripal_analysis_expression/content_types/protocol/sync') ?>">Protocol syncing page</a>
          to sync array designs. </p></li>
          <li><p><b>Manually Add An Array Design</b>: If your protocol is not already present in the Chado database
          you can create a protocol using the <a href="<?php print url('node/add/chado-protocol') ?>">Create Protocol page</a>.
          Once saved, the array design will be present in Chado and also "synced".</p>
      </ol>
    </li>
  </ul>

  <h3>Features of this Module:</h3>
  <p>Aside from array design page setup (as described in the Setup section above),
     The Tripal Expression module also provides the following functionality</p>
  <ul>
    <li><p><b>Simple Search Tool</b>: A <?php print l('simple search tool','chado/protocol') ?> is provided for 
    finding protocols. This tool relies on Drupal Views. <a href="https://www.drupal.org/project/views">Drupal Views</a>
    which must be installed to see the search tool. Look for it in the navigation menu under the item "Search Data". </p></li>
  </ul>   
 
 


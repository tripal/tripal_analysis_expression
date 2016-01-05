<?php 

$biomaterial = $variables['node']->biomaterial;

$all_relationships = $biomaterial->all_relationships;
$object_rels = $all_relationships['object'];
$subject_rels = $all_relationships['subject'];

if (count($object_rels) > 0 or count($subject_rels) > 0) { ?>
  <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc"></div> <?php

  // The subject relationships
  foreach ($subject_rels as $rel_type => $rels) {
    foreach ($rels as $obj_type => $objects) { ?>
      <p>The biomaterial <?php print $biomaterial->name;?> is <?php print $rel_type ?> the following biomaterials: <?php 

      $headers = array('Biomaterial Name', 'Organism', 'Biomaterial Provider');
      
      $rows = array();

      foreach ($objects as $object) {
        // Linke the biomaterial to it's node.
        $biomaterial_name = $object->record->object_id->name;
        if (property_exists($object->record, 'nid')) {
          $biomaterial_name = l($biomaterial_name, "node/" . $object->record->nid, array('attributes' => array('target' => "_blank"))); //*** what does this even do?
        }
        $organism = $object->record->object_id->name;
        $organism_name = $organism->genus . " " . $organism->species;
        if (property_exists($organism, 'nid')) {
          $organism_name = l("<i>" . $organism->genus . " " . $organism->species . "</i>", "node/" . $organism->nid, array('html' => TRUE));
        }
        $contact = $object->record->object_id->name;
        $contact_name = $contact->name;
        if (property_exists($contact, 'nid')) { //*** don't know if this will work
          $contact_name = l("<i>" . $contact->name . "</i>", "node/" . $contact->nid, array('html' => TRUE));
        }
        $rows[] = array(
          array('data' => $biomaterial_name, 'width' => '30%'), 
          array('data' => $object->record->object_id->name, 'width' => '30%'), 
          array('data' => $organism_name, 'width' => '30%'), 
          array('data' =>           
      }
}
}
}


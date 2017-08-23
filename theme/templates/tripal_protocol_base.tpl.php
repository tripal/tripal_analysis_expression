<?php
// This conditional is added to prevent errors in the array design TOC admin page.
if (property_exists($variables['node'], 'protocol')) {
  $protocol = $variables['node']->protocol;
  $protocol = chado_expand_var($protocol, 'field', 'protocol.protocoldescription');
  $protocol = chado_expand_var($protocol, 'field', 'protocol.hardwaredescription');
  $protocol = chado_expand_var($protocol, 'field', 'protocol.softwaredescription');
  //$protocol = chado_expand_var($protocol, 'field', 'arraydesign.description');
  ?>

    <div class="tripal_protocol-data-block-desc tripal-data-block-desc"></div>

  <?php

  $headers = [];
  $rows = [];

  // The protocol name.

  $rows[] = [
    [
      'data' => 'Protocol',
      'header' => TRUE,
      'width' => '20%',
    ],
    '<i>' . $protocol->name . '</i>',
  ];

  // The protocol type
  $rows[] = [
    [
      'data' => 'Protocol Type',
      'header' => TRUE,
      'width' => '20%',
    ],
    '<i>' . $protocol->type_id->name . '</i>',
  ];

  // The protocol link.
  if ($protocol->uri) {
    $rows[] = [
      [
        'data' => 'Protocol Link', //*** Don't know what to call this
        'header' => TRUE,
        'width' => '20%',
      ],
      '<i><a href="' . $protocol->uri . '"> Protocol </a></i>',
    ];
  }

  // The protocol link.
  if ($protocol->pub_id) {
    $rows[] = [
      [
        'data' => 'Publication',
        'header' => TRUE,
        'width' => '20%',
      ],
      '<i>' . l($protocol->pub_id->title, '/node/' . $protocol->pub_id->nid) . '</i>',
    ];
  }

  // allow site admins to see the protocol ID
  if (user_access('view ids')) {
    // Protocol ID
    $rows[] = [
      [
        'data' => 'Protocol ID',
        'header' => TRUE,
        'class' => 'tripal-site-admin-only-table-row',
      ],
      [
        'data' => $protocol->protocol_id,
        'class' => 'tripal-site-admin-only-table-row',
      ],
    ];
  }

  // Generate the table of data provided above. 
  // Generate the table of data provided above. 
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_arraydesign-table-base',
      'class' => 'tripal-arraydesign-data-table tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  // Print the table and the description.
  print theme_table($table);

  // Print the protocol description.
  if ($protocol->protocoldescription) {
    print '<h1> Protocol Description </h1>'; ?>
      <div style="text-align: justify"><?php print $protocol->protocoldescription ?></div>
      <br> <?php
  }

  // Print the hardware description.
  if ($protocol->hardwaredescription) {
    print '<h1> Hardware Description </h1>'; ?>
      <div style="text-align: justify"><?php print $protocol->hardwaredescription ?></div>
      <br> <?php
  }

  // Print the software description.
  if ($protocol->softwaredescription) {
    print '<h1> Software Description </h1>'; ?>
      <div style="text-align: justify"><?php print $protocol->softwaredescription ?></div>
      <br> <?php
  }

  ?>


  <?php
}
?>


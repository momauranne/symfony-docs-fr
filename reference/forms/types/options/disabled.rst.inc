disabled
~~~~~~~~
**type**: ``boolean``  **default**: ``false``

    Si vous ne voulez pas qu'un utilisateur puisse modifier la valeur d'un champ,
	vous pouvez d�finir l'option ``disabled`` � true. Toute valeur soumise sera
	ignor�e.
	
.. code-block:: php

    use Symfony\Component\Form\TextField
      
    $field = new TextField('status', array(
        'data' => 'Old data',
        'disabled' => true,
    ));
    $field->submit('New data');

    // prints "Old data"
    echo $field->getData();


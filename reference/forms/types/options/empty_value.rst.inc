empty_value
~~~~~~~~~~~

**type**: ``string`` ou ``Boolean``

Cette option d�termine si une option � vide � sp�cial (ex : � Choisissez une option �)
soit apparaitre en haut de la liste d�roulante. Cette option ne s'applique que si les
deux option ``expanded`` et ``multiple`` ont �t� d�finies � false.

* Ajoute une valeur vide avec � Choisissez une option � comme texte::

    $builder->add('states', 'choice', array(
        'empty_value' => 'Choisissez une option',
    ));

* Garantie qu'aucune option � vide � n'est affich�e::

    $builder->add('states', 'choice', array(
        'empty_value' => false,
    ));

Si vous ne d�finissez pas l'option ``empty_value``, alors une option vide (sans texte)
sera automatiquement ajout�e si et seulement si l'option ``required`` est � false::

    // Une option vide (sans texte) sera ajout�e
    $builder->add('states', 'choice', array(
        'required' => false,
    ));
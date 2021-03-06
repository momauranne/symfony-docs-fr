.. index::
   single: Validation

Validation
==========

La validation est une t�che tr�s commune dans les applications web. Les donn�es 
saisies dans les formulaires doivent �tre valid�es. Les donn�es doivent �galement
�tre valid�es avant d'�tre �crites dans une base de donn�es ou transmises � un
service Web.

Symfony2 est livr� avec un composant `Validator`_ qui rend cette t�che facile et
transparente. Ce composant est bas� sur la `sp�cification JSR303`_.
Quoi ? Une sp�cification Java en PHP? Vous avez bien entendu, mais ce n'est pas
aussi terrible que �a en a l'air. Regardons comment elle peut �tre utilis�e en PHP.

.. index:
   single: Validation; Les bases

Les Bases de la Validation
--------------------------

La meilleure fa�on de comprendre la validation est de la voir en action. Pour
commencer, supposons que vous avez cr�� un bon vieil objet PHP que vous avez
besoin d'utiliser quelque part dans votre application :

.. code-block:: php

    // src/Acme/BlogBundle/Entity/Author.php
    namespace Acme\BlogBundle\Entity;
    
    class Author
    {
        public $name;
    }

Jusqu'� pr�sent, c'est juste une classe ordinaire qui est utile dans votre application.
L'objectif de la validation est de vous dire si oui ou non les donn�es d'un objet
sont valides. Pour que cela fonctionne, vous allez configurer une liste de r�gles
(appel�e :ref:`constraints<validation-constraints>`) que l'objet doit respecter pour
�tre valide. Ces r�gles peuvent �tre sp�cifi�es via un certain nombre de formats
(YAML, XML, les annotations, ou PHP).

Par exemple, pour garantir que la propri�t� ``$name`` ne soit pas vide, ajoutez le code
suivant :

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                name:
                    - NotBlank: ~

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank()
             */
            public $name;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="name">
                    <constraint name="NotBlank" />
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
            }
        }

.. tip::

    Les propri�t�s prot�g�es et priv�es peuvent �galement �tre valid�es, ainsi
    que les m�thodes � getter � (voir validator-constraint-targets).
	
.. index::
   single: Validation; Utiliser le validator

Utiliser le Service ``validator``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ensuite, pour vraiment valider un objet ``Author``, utilisez la m�thode ``validate``
du service validator (class :class:`Symfony\\Component\\Validator\\Validator`).
Le travail du ``validator`` est simple : lire les contraintes (r�gles) d'une classe
et v�rifier si les donn�es sur l'objet satisfont ces contraintes ou non. Si la
validation �choue, un tableau d'erreurs est retourn�. Prenez cet exemple simple
d'une action d'un contr�leur :

.. code-block:: php

    use Symfony\Component\HttpFoundation\Response;
    use Acme\BlogBundle\Entity\Author;
    // ...

    public function indexAction()
    {
        $author = new Author();
        // ... do something to the $author object

        $validator = $this->get('validator');
        $errorList = $validator->validate($author);

        if (count($errorList) > 0) {
            return new Response(print_r($errorList, true));
        } else {
            return new Response('The author is valid! Yes!');
        }
    }

Si la propri�t� ``$name`` est vide, vous allez voir le message d'erreur suivant :

.. code-block:: text

    Acme\BlogBundle\Author.name:
        This value should not be blank

Si vous ins�rez une valeur dans la propri�t� ``name``, le message de succ�s va appara�tre.

.. tip::

	La plupart du temps, vous n'aurez pas � interagir directement avec le
        service ``validator`` ou besoin de vous inqui�ter concernant l'affichage des erreurs.
        La plupart du temps, vous allez utiliser la validation indirectement lors
        de la soumission des donn�es du formulaire. Pour plus d'informations,
        consultez le :ref:`book-validation-forms`.
    
Vous pouvez aussi passer une collection d'erreur � un template.

.. code-block:: php

    if (count($errorList) > 0) {
        return $this->render('AcmeBlogBundle:Author:validate.html.twig', array(
            'errorList' => $errorList,
        ));
    } else {
        // ...
    }

A l'int�rieur d'un template, vous pouvez afficher la liste des erreurs comme
vous voulez :
	

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Author/validate.html.twig #}

        <h3>The author has the following errors</h3>
        <ul>
        {% for error in errorList %}
            <li>{{ error.message }}</li>
        {% endfor %}
        </ul>

    .. code-block:: html+php

        <!-- src/Acme/BlogBundle/Resources/views/Author/validate.html.php -->

        <h3>The author has the following errors</h3>
        <ul>
        <?php foreach ($errorList as $error): ?>
            <li><?php echo $error->getMessage() ?></li>
        <?php endforeach; ?>
        </ul>

.. note::

    Chaque erreur de validation (appel�e une � violation de contrainte �), est
    repr�sent�e par un objet :class:`Symfony\\Component\\Validator\\ConstraintViolation`.

.. index::
   single: Validation; Validation et formulaires

.. _book-validation-forms:

Validation et Formulaires
~~~~~~~~~~~~~~~~~~~~~~~~~

Le service ``validator`` peut �tre utilis� � tout moment pour valider n'importe
quel objet. En r�alit�, vous travaillerez le plus souvent avec le ``validator``
indirectement lorsque vous utilisez les formulaires. La biblioth�que de formulaires
de Symfony utilise le service ``validator`` en interne pour valider l'objet
apr�s que les valeurs ont �t� soumises. Les violations de contraintes sur
l'objet sont converties en objets ``FieldError`` qui peuvent �tre facilement affich�s
dans votre formulaire. Le processus de soumission d'un formulaire typique ressemble
au code suivant :

    use Acme\BlogBundle\Entity\Author;
    use Acme\BlogBundle\Form\AuthorType;
    use Symfony\Component\HttpFoundation\Request;
    // ...

    public function updateAction(Request $request)
    {
        $author = new Acme\BlogBundle\Entity\Author();
        $form = $this->createForm(new AuthorType(), $author);
        
        if ($request->getMethod() == 'POST') {
            $form->bindRequest($request);
            
            if ($form->isValid()) {
                // the validation passed, do something with the $author object
                
                return $this->redirect($this->generateUrl('...'));
            }
        }
        
        return $this->render('BlogBundle:Author:form.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. note::

    Cet exemple utilise une classe de formulaire ``AuthorType``, qui n'est pas montr�e ici.
	
Pour plus d'informations, voir le chapitre :doc:`Forms</book/forms>`.

.. index::
   pair: Validation; Configuration

.. _book-validation-configuration:

Configuration
-------------

Le validateur Symfony2 est activ�e par d�faut, mais vous devez activer explicitement
les annotations, si vous voulez les utiliser pour sp�cifier vos contraintes :

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            validation: { enable_annotations: true }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <framework:config>
            <framework:validation enable_annotations="true" />
        </framework:config>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array('validation' => array(
            'enable_annotations' => true,
        )));

.. index::
   single: Validation; Constraints

.. _validation-constraints:

Contraintes
-----------

Le ``validator`` est con�u pour valider des objets selon des *contraintes* 
(r�gles). Afin de valider un objet, il suffit d'associer une ou plusieurs contraintes
� sa classe et ensuite de le passer au service ``validator``.

Dans les coulisses, une contrainte est simplement un objet PHP qui d�clare une r�gle.
Dans la vraie vie, une contrainte pourrait �tre : � Le g�teau ne doit pas �tre br�l� �.
En Symfony2, les contraintes sont similaires : ce sont des garanties qu'une condition
est vraie. Pour une valeur donn�e, une contrainte vous dira si cette valeur ob�it 
aux r�gles ou non.
 

Contraintes support�es
~~~~~~~~~~~~~~~~~~~~~~

Symfony2 est fourni avec un grand nombre des contraintes les plus utiles habituellement.

.. include:: /reference/constraints/map.rst.inc

Vous pouvez aussi cr�er vos propres contraintes. Ce sujet est abord� dans l'article �:doc:`/cookbook/validation/custom_constraint`� du cookbook.

.. index::
   single: Validation; Constraints configuration

.. _book-validation-constraint-configuration:

Configuration de contraintes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Certaines contraintes, comme :doc:`NotBlank</reference/constraints/NotBlank>`,
sont simples alors que d'autres, comme la contrainte :doc:`Choice</reference/constraints/Choice>`
ont plusieurs options de configuration disponibles. Supposons que la classe
``Author`` a une autre propri�t�, ``gender`` qui peut prendre comme valeur
� homme � ou � femme � :

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                gender:
                    - Choice: { choices: [homme, femme], message: Choose a valid gender. }

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice(
             *     choices = { "homme", "femme" },
             *     message = "Choose a valid gender."
             * )
             */
            public $gender;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="gender">
                    <constraint name="Choice">
                        <option name="choices">
                            <value>homme</value>
                            <value>femme</value>
                        </option>
                        <option name="message">Choose a valid gender.</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>


    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $gender;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('gender', new Choice(array(
                    'choices' => array('homme', 'femme'),
                    'message' => 'Choose a valid gender.',
                )));
            }
        }

.. _validation-default-option:

Les options d'une contrainte peuvent toujours �tre pass�es en tant que tableau. Certaines contraintes,
cependant, vous permettent �galement de passer la valeur d'une option � *par d�faut* �
au lieu du tableau. Dans le cas de la contrainte ``Choice``, les options de ``choices``
peuvent �tre sp�cifi�es de cette mani�re.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                gender:
                    - Choice: [homme, femme]

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice({"homme", "femme"})
             */
            protected $gender;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="Acme\BlogBundle\Entity\Author">
                <property name="gender">
                    <constraint name="Choice">
                        <value>homme</value>
                        <value>femme</value>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\Choice;

        class Author
        {
            protected $gender;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('gender', new Choice(array('homme', 'femme')));
            }
        }

Ceci a pour unique but de rendre la configuration de l'option la plus probable
plus courte et plus rapide.

Si jamais vous n'�tes pas certain de la fa�on dont sp�cifier une option, soit v�rifiez la documentation de l'API
pour la contrainte concern�e, soit jouez la s�curit� en passant toujours dans un tableau d'options
(la premi�re m�thode indiqu�e ci-dessus).

.. index::
   single: Validation; Constraint targets

.. _validator-constraint-targets:

Objectifs des contraintes
-------------------------

Les contraintes peuvent �tre appliqu�es � une propri�t� de classe (par ex. ``name``) ou une
m�thode publique getter (par ex. ``getFullName``). La premi�re m�thode est la plus
commune et facile � utiliser, mais la seconde vous permet de sp�cifier des r�gles
de validation plus complexes.

.. index::
   single: Validation; Property constraints

.. _validation-property-target:

Propri�t�s

Valider des propri�t�s de classe est la technique de validation la plus basique. Symfony2
vous permet de valider des propri�t�s priv�es, prot�g�es ou publiques. Le prochain
exemple vous montre comment configurer la propri�t� ``$firstName`` d'une classe ``Author``
pour avoir au moins 3 caract�res.

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            properties:
                firstName:
                    - NotBlank: ~
                    - MinLength: 3

    .. code-block:: php-annotations

        // Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank()
             * @Assert\MinLength(3)
             */
            private $firstName;
        }


    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\BlogBundle\Entity\Author">
            <property name="firstName">
                <constraint name="NotBlank" />
                <constraint name="MinLength">3</constraint>
            </property>
        </class>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\MinLength;

        class Author
        {
            private $firstName;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('firstName', new NotBlank());
                $metadata->addPropertyConstraint('firstName', new MinLength(3));
            }
        }

.. index::
   single: Validation; Getter constraints

Getters
~~~~~~~

Les contraintes peuvent �galement �tre appliqu�es � la valeur de retour d'une m�thode. Symfony2
vous permet d'ajouter une contrainte � toute m�thode publique dont le nom commence par
� get � ou � is �. Dans ce guide, ces deux types de m�thodes sont d�sign�es 
comme � getters �.

L'avantage de cette technique est qu'elle vous permet de valider votre objet
dynamiquement. Par exemple, supposons que vous voulez vous assurer qu'un champ
mot de passe ne correspond pas au pr�nom de l'utilisateur (pour des raisons de s�curit�).
Vous pouvez le faire en cr�ant une m�thode ``isPasswordLegal``, puis en sp�cifiant que
cette m�thode doit retourner ``true`` :

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\Author:
            getters:
                passwordLegal:
                    - "True": { message: "Le mot de passe et le pr�nom doivent �tre diff�rents" }

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\True(message = "Le mot de passe et le pr�nom doivent �tre diff�rents")
             */
            public function isPasswordLegal()
            {
                // return true or false
            }
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\BlogBundle\Entity\Author">
            <getter property="passwordLegal">
                <constraint name="True">
                    <option name="message">Le mot de passe et le pr�nom doivent �tre diff�rents</option>
                </constraint>
            </getter>
        </class>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/Author.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\True;

        class Author
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addGetterConstraint('passwordLegal', new True(array(
                    'message' => 'Le mot de passe et le pr�nom doivent �tre diff�rents',
                )));
            }
        }

Maintenant, cr�ez la m�thode ``isPasswordLegal()``, et faites ce dont vous avez besoin:: 

    public function isPasswordLegal()
    {
        return ($this->firstName != $this->password);
    }

.. note::

    Les plus perspicaces d'entre vous auront remarqu� que le pr�fixe du getter
    (� get � ou � is �) est omis dans le mapping. Cela vous permet de d�placer la
    contrainte sur une propri�t� du m�me nom plus tard (ou vice versa) sans
    changer votre logique de validation.
	
.. _validation-class-target:

Classes	
~~~~~~~

Certaines contraintes s'appliquent � toute la classe qui est valid�e. Par exemple,
la contrainte :doc:`Callback</reference/constraints/Callback>` est une contrainte
g�n�rique qui s'applique � la classe elle-m�me. Lorsque cette classe est valid�e,
les m�thodes sp�cifi�es par cette contrainte sont simplement ex�cut�es pour personnaliser
encore plus la validation
	
.. _book-validation-validation-groups:

Groupes de Validation
---------------------

Jusqu'ici, vous avez �t� en mesure d'ajouter des contraintes � une classe et demander si oui ou
non cette classe satisfait toutes les contraintes d�finies. Dans certains cas, cependant,
vous aurez besoin de valider un objet en ne prenant en compte que *certaines* des contraintes
de la classe. Pour ce faire, vous pouvez organiser chaque contrainte en un ou plusieurs
� groupes de validation �, et ensuite appliquer la validation sur un groupe de
contraintes seulement.

Par exemple, supposons que vous avez une classe  ``User``, qui est utilis�e � la fois quand un
utilisateur s'enregistre et quand un utilisateur met � jour son profil plus tard : 

.. configuration-block::

    .. code-block:: yaml

        # src/Acme/BlogBundle/Resources/config/validation.yml
        Acme\BlogBundle\Entity\User:
            properties:
                email:
                    - Email: { groups: [registration] }
                password:
                    - NotBlank: { groups: [registration] }
                    - MinLength: { limit: 7, groups: [registration] }
                city:
                    - MinLength: 2

    .. code-block:: php-annotations

        // src/Acme/BlogBundle/Entity/User.php
        namespace Acme\BlogBundle\Entity;

        use Symfony\Component\Security\Core\User\UserInterface;
        use Symfony\Component\Validator\Constraints as Assert;

        class User implements UserInterface
        {
            /**
            * @Assert\Email(groups={"registration"})
            */
            private $email;

            /**
            * @Assert\NotBlank(groups={"registration"})
            * @Assert\MinLength(limit=7, groups={"registration"})
            */
            private $password;

            /**
            * @Assert\MinLength(2)
            */
            private $city;
        }

    .. code-block:: xml

        <!-- src/Acme/BlogBundle/Resources/config/validation.xml -->
        <class name="Acme\BlogBundle\Entity\User">
            <property name="email">
                <constraint name="Email">
                    <option name="groups">
                        <value>registration</value>
                    </option>
                </constraint>
            </property>
            <property name="password">
                <constraint name="NotBlank">
                    <option name="groups">
                        <value>registration</value>
                    </option>
                </constraint>
                <constraint name="MinLength">
                    <option name="limit">7</option>
                    <option name="groups">
                        <value>registration</value>
                    </option>
                </constraint>
            </property>
            <property name="city">
                <constraint name="MinLength">7</constraint>
            </property>
        </class>

    .. code-block:: php

        // src/Acme/BlogBundle/Entity/User.php
        namespace Acme\BlogBundle\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\Email;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use Symfony\Component\Validator\Constraints\MinLength;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('email', new Email(array(
                    'groups' => array('registration')
                )));

                $metadata->addPropertyConstraint('password', new NotBlank(array(
                    'groups' => array('registration')
                )));
                $metadata->addPropertyConstraint('password', new MinLength(array(
                    'limit'  => 7,
                    'groups' => array('registration')
                )));

                $metadata->addPropertyConstraint('city', new MinLength(3));
            }
        }

Avec cette configuration, il y a deux groupes de validation :

* ``Default`` - contient les contraintes non affect�es � tout autre groupe ;

* ``registration`` - contient les contraintes sur les champs ``email`` and ``password``
  seulement.

Pour dire au validateur d'utiliser un groupe sp�cifique, passer un ou plusieurs noms de groupe
comme deuxi�me argument de la m�thode ``validate()`` ::

    $errorList = $validator->validate($author, array('registration'));

Bien s�r, vous travaillerez bien souvent indirectement avec la validation via la biblioth�que de
formulaire. Pour plus d'informations sur la fa�on d'utiliser les groupes de validation
� l'int�rieur des formulaires, voir :ref:`book-forms-validation-groups`.

.. index::
   single: Validation; Validating raw values

.. _book-validation-raw-values:

Valider des valeurs et des tableaux
-----------------------------------

Jusqu'ici, vous avez vu comment valider des objets entiers. Mais souvent,
vous voudrez juste valider une simple valeur, comme par exemple v�rifier qu'une
chaine de caract�res est une adresse email valide. C'est en fait tr�s facile �
faire. Dans un contr�leur, �a ressemble � ceci::

    // Ajouter ceci en haut de votre classe
    use Symfony\Component\Validator\Constraints\Email;
    
    public function addEmailAction($email)
    {
        $emailConstraint = new Email();
        // toutes les "options" de contrainte sont d�finies comme suit
        $emailConstraint->message = 'Invalid email address';

        // utiliser le validator pour valider la valeur
        $errorList = $this->get('validator')->validateValue($email, $emailConstraint);

        if (count($errorList) == 0) {
            // c'est une adresse valide, faire quelque chose
        } else {
            // ce n'est *pas* une adresse valide
            $errorMessage = $errorList[0]->getMessage()
            
            // faire quelque chose avec l'erreur
        }
        
        // ...
    }

En appelant ``validateValue`` du validator, vous pouvez passer une valeur brute et
l'objet contrainte dont vous voulez valider la valeur. Une liste compl�te des
contraintes disponibles - ainsi que le nom de classe complet de chaque contrainte -
est disponible dans le chapitre :doc:`contraintes</reference/constraints>`.

La m�thode ``validateValue`` retourne un objet :class:`Symfony\\Component\\Validator\\ConstraintViolationList`
qui se comporte comme un tableau d'erreurs. Chaque erreur de la collection est un
objet :class:`Symfony\\Component\\Validator\\ConstraintViolation`, qui contient
le message d'erreur dans sa m�thode `getMessage`.

Le mot de la fin
----------------

Le ``validator`` de Symfony2 est un outil puissant qui peut �tre un atout pour
garantir que les donn�es de n'importe quel objet est � valide �. La puissance
de la validation r�side dans les � contraintes �, qui sont des r�gles que vous
pouvez appliquer aux propri�t�s ou aux m�thodes getter de votre objet. Et tandis
que vous utiliserez plus commun�ment le syst�me de validation indirectement
lors de l'utilisation des formulaires, n'oubliez pas qu'il peut �tre utilis� partout
pour valider n'importe quel objet.

En savoir plus avec le Cookbook
-------------------------------

* :doc:`/cookbook/validation/custom_constraint`

.. _Validator: https://github.com/symfony/Validator
.. _sp�cification JSR303: http://jcp.org/en/jsr/detail?id=303

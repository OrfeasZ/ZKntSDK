#pragma once

#include <Common.hpp>

namespace Gameplay {
    // Size: 0x4
    enum class EDashCoverSwapFacingMode : int32_t {
        KeepDashFacing = 0,
        SwapFacingToTargetCorner = 1,
        InputDrivenDynamicFacing = 2,
    };

    // Size: 0x1
    enum class EAgilityActionAttributeSearchType : int8_t {
        LowestValue = 0,
        HighestValue = 1,
    };

    // Size: 0x1
    enum class EAgilityActionLaunchVelocityClampBehavior : int8_t {
        None = 0,
        Signed = 1,
        Unsigned = 2,
    };

    // Size: 0x1
    enum class EAgilityActionReferencePointStepOperation : int8_t {
        CapsuleTransform = 0,
        BoneTransform = 1,
        CameraTransform = 2,
        MoveInputOrientation = 3,
        VelocityOrientation = 4,
        VelocityTranslation = 5,
        SourceMarkupWorldNormalized = 6,
        SourceMarkupLocalNormalized = 7,
        SourceMarkupClosestPoint = 8,
        SourceMarkupConnectionLink = 9,
        SourceMarkupPlaneIntersection = 10,
        TargetMarkupWorldNormalized = 11,
        TargetMarkupLocalNormalized = 12,
        TargetMarkupClosestPoint = 13,
        TargetMarkupConnectionLink = 14,
        TargetMarkupPlaneIntersection = 15,
        WorldTranslation = 16,
        LocalTranslation = 17,
        Rotation = 18,
        AffineInverse = 19,
        TranslationDirectionToRotation = 20,
        RotationToTranslationDirection = 21,
        PlanarizedRotation = 22,
        RayTrace = 23,
        SafeCapsuleTransformTowardsLocalRight = 24,
    };

    // Size: 0x1
    enum class EAgilityActionReferencePointStepMethod : int8_t {
        Set = 0,
        Apply = 1,
        Offset = 2,
        Scale = 3,
    };

    // Size: 0x4
    enum class EHumanoidBoneType : int32_t {
        Hip = 0,
        Torso = 1,
        Head = 2,
        RightKnee = 3,
        LeftKnee = 4,
        RightClavicle = 5,
        LeftClavicle = 6,
        RightElbow = 7,
        LeftElbow = 8,
        BackAttacher = 9,
        RightHandAttacher = 10,
        RightIKHandAttacher = 11,
        LeftHandAttacher = 12,
        LeftIKHandAttacher = 13,
        RightFoot = 14,
        LeftFoot = 15,
        FreeAttacher_Ground = 16,
        HeadEnd = 17,
        Ground = 18,
        NpcVisionEffector = 19,
        RightWrist = 20,
        LeftWrist = 21,
        RightShoulder = 22,
        LeftShoulder = 23,
        RightFemur = 24,
        LeftFemur = 25,
        PropAttacher = 26,
        Spine03 = 27,
        Nose = 28,
    };

    // Size: 0x4
    enum class ECloseCombatReaction : int32_t {
        eCCR_Block = 0,
        eCCR_BlockAndCounter = 1,
        eCCR_SideStep = 2,
        eCCR_SideStepAndCounter = 3,
        eCCR_Wait = 4,
        eCCR_Parry = 5,
    };

    // Size: 0x1
    enum class EVehicleDriftingDeactivationConditionType : int8_t {
        ForwardSpeed = 0,
        LateralSpeed = 1,
        AngularSpeed = 2,
        SteeringValue = 3,
        LateralSpeedUp = 4,
        AngularSpeedUp = 5,
        CounterSteering = 6,
        HandbrakeOff = 7,
        BrakesOff = 8,
    };

    // Size: 0x4
    enum class EDashTargetingBehaviour : int32_t {
        Default = 0,
        ClosestTargetToPlayer = 1,
        FurthestTargetToPlayer = 2,
    };

    // Size: 0x4
    enum class EDashStancePolicy : int32_t {
        TryKeepStance = 0,
        ChangeAccordingToCoverType = 1,
    };

    // Size: 0x1
    enum class EAnimationLocomotionClimbingType : int8_t {
        None = 0,
        Ladder = 1,
        Drainpipe = 2,
    };

    // Size: 0x1
    enum class EClimbingTransitionSide : int8_t {
        Bottom = 0,
        Top = 1,
        Left = 2,
        Right = 3,
    };

    // Size: 0x4
    enum class EAIFlightRecordChannel : int32_t {
        General = 0,
        Plan_Primary = 1,
        Plan_Secondary = 2,
        Plan_Tertiary = 3,
        HTN = 4,
        HTNPinned = 5,
        Roles = 6,
        Senses = 7,
        PerceptionModifiers = 8,
        TagAdd = 9,
        TagRemove = 10,
        CooldownAdd = 11,
        CooldownRemove = 12,
        Investigation = 13,
        Storyline = 14,
        Locomotion = 15,
        Pathfinding = 16,
        LookAt = 17,
        StateCommunication = 18,
        AnimationAct = 19,
        Dialogue = 20,
        Communication = 21,
        StateIcon = 22,
        TacticalEspionageArea = 23,
        Count = 24,
    };

    // Size: 0x4
    enum class EAIFlightRecorderEntryType : int32_t {
        Message = 0,
        Headline = 1,
        Warning = 2,
        Error = 3,
        Count = 4,
    };

    // Size: 0x1
    enum class EActionDirection : int8_t {
        None = 0,
        Forward = 1,
        Back = 2,
        Left = 3,
        Right = 4,
        Any = 5,
    };

    // Size: 0x1
    enum class EActionKeywordsType : int8_t {
        Parry = 0,
        CombatRush = 1,
        Grab = 2,
        Agility = 3,
        Locomotion = 4,
        LocomotionApproach = 5,
        EquipItem = 6,
        MeleeAttack = 7,
        GadgetView = 8,
        ContextualItemThrow = 9,
        CloseCombatPairedAction = 10,
        FakeSurrender = 11,
        Sidestep = 12,
        CoverTakedown = 13,
        JumpAway = 14,
        CoverTransition = 15,
        EnterLadderClimb = 16,
        LeaveLadderClimb = 17,
        Crouch = 18,
        Uncrouch = 19,
        Interact = 20,
        ThrowItem = 21,
        Pickpocket = 22,
        BlendingIn = 23,
        UseDoor = 24,
        PickUpItem = 25,
        ActivateGadget = 26,
        DropDown = 27,
        StartStrafing = 28,
        StopStrafing = 29,
        StartAccelerating = 30,
        StopAccelerating = 31,
        Stagger = 32,
        TrespassingStanceChange = 33,
        AnimationAct = 34,
        AimFirearm = 35,
        ShootFirearm = 36,
        GrabAttack = 37,
        GrabRush = 38,
        ReloadFirearm = 39,
        Block = 40,
        Dash = 41,
    };

    // Size: 0x1
    enum class EAdjacentCoverType : int8_t {
        Regular = 0,
        Corner = 1,
        InnerCorner = 2,
    };

    // Size: 0x4
    enum class EAgilityActionAllowance : int32_t {
        AllowWhileUsingGadget = 1,
        AllowWhileFalling = 2,
        AllowWhileAiming = 4,
        AllowWhileInConfrontation = 8,
        AllowWhileGrounded = 16,
        AllowWhilePerformingSelf = 32,
    };

    // Size: 0x1
    enum class EAgilityActionApproachingMovementDisplacementErrorCorrectionType : int8_t {
        None = 0,
        BlendByRemainingDistance = 1,
        BlendByTraveledDistance = 2,
        BlendByRemainingRatio = 3,
        BlendByTraveledRatio = 4,
    };

    // Size: 0x1
    enum class EAgilityActionApproachingMovementProjectionType : int8_t {
        None = 0,
        SourceMarkupPath = 1,
        TargetMarkupPath = 2,
    };

    // Size: 0x1
    enum class EAgilityActionApproachingMovementType : int8_t {
        Stationary = 0,
        Moving = 1,
        Dynamic = 2,
    };

    // Size: 0x1
    enum class EAgilityActionAttributeOperatorType : int8_t {
        Distance2D = 0,
        Distance3D = 1,
        DistanceLocalX = 2,
        DistanceLocalY = 3,
        DistanceLocalZ = 4,
        AbsOrientationYawDelta = 5,
        InputDirectionMatch = 6,
        LastNonZeroInputDirectionMatch = 7,
    };

    // Size: 0x1
    enum class EAgilityActionAttributeSelectionInfluence : int8_t {
        None = 0,
        LowerValuePriority = 1,
        HigherValuePriority = 2,
    };

    // Size: 0x1
    enum class EAgilityActionCollectionEvaluationType : int8_t {
        All = 0,
        Any = 1,
    };

    // Size: 0x1
    enum class EAgilityActionConditionConstraintType : int8_t {
        DistanceLower = 0,
        DistanceGreater = 1,
        DistanceXYLower = 2,
        DistanceXYGreater = 3,
        DistanceZLower = 4,
        DistanceZGreater = 5,
        RelativeLower = 6,
        RelativeGreater = 7,
        OrientationLower = 8,
        OrientationGreater = 9,
        AbsOrientationLower = 10,
        AbsOrientationGreater = 11,
        FacingSignLower = 12,
        FacingSignGreater = 13,
        InsideSlice = 14,
        BehindOfForwardPlane = 15,
        FrontOfForwardPlane = 16,
        InputMagnitudeLower = 17,
        InputMagnitudeGreater = 18,
        InputDirectionMatch = 19,
        InputDirectionDeltaGreater = 20,
        InputDirectionDeltaLower = 21,
        LastNonZeroInputDirectionMatch = 22,
        VelocityMagnitudeLower = 23,
        VelocityMagnitudeGreater = 24,
        VelocityDirectionMatch = 25,
        HasAngularVelocity = 26,
        PerformingAgilityActionWithContentHint = 27,
        HasOverlappingHumanoidInBox = 28,
        HasOverlappingCollisionInBox = 29,
        OuterConditionsOr = 30,
        OuterConditionsAnd = 31,
        LiteralTrue = 32,
        LiteralFalse = 33,
    };

    // Size: 0x1
    enum class EAgilityActionConnectionDebugDrawType : int8_t {
        None = 0,
        Line = 1,
        DottedLine = 2,
        Sphere = 3,
        Box = 4,
    };

    // Size: 0x4
    enum class EAgilityActionConnectionNavLinkFlag : int32_t {
        ContextualJump = 1,
    };

    // Size: 0x1
    enum class EAgilityActionConnectionTargetType : int8_t {
        Self = 0,
        Other = 1,
    };

    // Size: 0x1
    enum class EAgilityActionEnvironmentContextTarget : int8_t {
        SourceEnvironmentContext = 0,
        TargetEnvironmentContext = 1,
    };

    // Size: 0x1
    enum class EAgilityActionHumanoidState : int8_t {
        None = 0,
        Hang = 1,
        Sidle = 2,
        Ladder = 3,
        Drainpipe = 4,
        ZipLine = 5,
    };

    // Size: 0x4
    enum class EAgilityActionNativeHint : int32_t {
        ReorientCamera = 1,
        Illegal = 2,
        UnequipItem = 4,
        CreateObstacleWhenRestricted = 8,
        UseDefaultBoneCollisions = 16,
        ImmuneToStagger = 32,
        UnequipItemInstant = 64,
        AnimatedCamera = 128,
        InterestingCamera = 256,
    };

    // Size: 0x4
    enum class EAgilityActionPerformance : int32_t {
        AbortWhenOpportunityLost = 1,
        AbortWhenInputLost = 2,
        DismountWhenAborted = 4,
    };

    // Size: 0x4
    enum class EAgilityActionReferencePointStepFlag : int32_t {
        IgnoreRotation = 1,
        IgnoreTranslation = 2,
        IgnoreRotationIfIdentity = 4,
        IgnoreTranslationIfZero = 8,
        IgnoreTranslationsOnPositiveAxes = 16,
        IgnoreTranslationsOnNegativeAxes = 32,
        IgnoreTranslationIfConditionTrue = 64,
        IgnoreRotationIfConditionTrue = 128,
        IgnoreTranslationIfConditionFalse = 256,
        IgnoreRotationIfConditionFalse = 512,
    };

    // Size: 0x4
    enum class EAgilityActionRequirement : int32_t {
        RequireBeingGrounded = 1,
    };

    // Size: 0x4
    enum class EAgilityActionRestriction : int32_t {
        CombatNearby = 1,
        ReloadPrioritizedWhenCombatNearby = 2,
        CoreLogic = 4,
    };

    // Size: 0x1
    enum class EAgilityActionState : int8_t {
        NotAvailable = 0,
        OpportunityAvailable = 1,
        Performing = 2,
    };

    // Size: 0x1
    enum class EAgilityActionTransitionType : int8_t {
        Instant = 0,
        Timed = 1,
        AnimationDriven = 2,
        ApproachingMovement = 3,
        Launch = 4,
    };

    // Size: 0x1
    enum class EAgilityGoalIssuePolicy : int8_t {
        Input = 0,
        Automatic = 1,
        InputOrAutomatic = 2,
    };

    // Size: 0x4
    enum class EAgilityStartingStance : int32_t {
        Crouch_Idle = 0,
        Crouch_Walking = 1,
        Standing_Idle = 2,
        Standing_Walking = 3,
        Standing_Sprinting = 4,
    };

    // Size: 0x4
    enum class EAligningMovementIssuer : int32_t {
        Unknown = 0,
        Block = 1,
        MeleeAttack = 2,
        ItemThrow = 3,
        Grab = 4,
        CombatRush = 5,
        Sidestep = 6,
        Parry = 7,
        Doors = 8,
        CloseCombatPairedAction = 9,
        AttackFromAbove = 10,
        Pickpocket = 11,
        PrivateSpaceReaction = 12,
        Stumble = 13,
        VaultAttack = 14,
        FakeSurrender = 15,
    };

    // Size: 0x1
    enum class EAnimationActAbortCause : int8_t {
        Default = 0,
        ConcreteBag = 1,
        ActiveBluff = 2,
        PhoneDart = 3,
        Blinded = 4,
        Combat = 5,
    };

    // Size: 0x1
    enum class EAnimationActAbortReason : int8_t {
        None = 0,
        Urgent = 1,
        NonUrgent = 2,
        Snap = 3,
    };

    // Size: 0x1
    enum class EAnimationActLoopStance : int8_t {
        Standing = 0,
        Crouched = 1,
    };

    // Size: 0x1
    enum class EAnimationAttackFromAboveType : int8_t {
        Falling = 0,
        Edge = 1,
        Railing = 2,
        Hang = 3,
        Sidle = 4,
    };

    // Size: 0x1
    enum class EAnimationCombatState : int8_t {
        None = 0,
        InCombat = 1,
    };

    // Size: 0x1
    enum class EAnimationDashState : int8_t {
        NoDash = 0,
        Dashing_StandToCrouch = 1,
        Dashing_StandToStand = 2,
        Dashing_CrouchToCrouch = 3,
        Dashing_CrouchToStand = 4,
        Dashing_Combat = 5,
    };

    // Size: 0x1
    enum class EAnimationDazeType : int8_t {
        Smoke = 0,
        Electric = 1,
        Vision = 2,
        Physical = 3,
        Auditory = 4,
        TrapSetpiece = 5,
        BodyPartWeakPoint = 6,
        Glue = 7,
        Oil = 8,
    };

    // Size: 0x1
    enum class EAnimationEnvironmentalLockState : int8_t {
        Unlocked = 0,
        Sit_100cm = 1,
        Sit_020cm = 2,
        Sit_000cm = 3,
        Sit_Stool_080cm = 4,
        Sit_Chair = 5,
        Sit_Couch = 6,
        Lean_120cm = 7,
        Lean_100cm = 8,
        Lean_075cm = 9,
        Lean_Wall = 10,
        Stand = 11,
    };

    // Size: 0x1
    enum class EAnimationFirearmArmorHitState : int8_t {
        NoArmor = 0,
        ArmorHit = 1,
        ArmorDestroyed = 2,
    };

    // Size: 0x1
    enum class EAnimationFirearmDisarmSource : int8_t {
        CoreLogic = 0,
        HitReaction = 1,
        Stagger = 2,
    };

    // Size: 0x1
    enum class EAnimationFirearmPosture : int8_t {
        None = 0,
        Lowered = 1,
        Primed = 2,
        Raised = 3,
        AboveHead = 4,
        BlindFiringLeft = 5,
        BlindFiringRight = 6,
        LeaningLeft = 7,
        LeaningRight = 8,
    };

    // Size: 0x1
    enum class EAnimationGrabBreakFreeType : int8_t {
        None = 0,
        RegularGrab = 1,
        GrabRush = 2,
    };

    // Size: 0x1
    enum class EAnimationGrabStep : int8_t {
        ThrowingFromAttack = 0,
        RedirectThrow = 1,
        PreparingGrab = 2,
        Grabbing = 3,
        FailingGrab = 4,
        BreakingFree = 5,
        Throwing = 6,
        Attacking = 7,
        Rushing = 8,
        RushingImpact = 9,
        Taunting = 10,
    };

    // Size: 0x1
    enum class EAnimationGrabbedStep : int8_t {
        BeingThrownFromAttack = 0,
        PreparingGrab = 1,
        Grabbing = 2,
        Throwing = 3,
        BreakingFree = 4,
        BeingRedirectThrown = 5,
        BeingRushed = 6,
        GettingHit = 7,
        Unused_2 = 8,
        GettingShot = 9,
    };

    // Size: 0x1
    enum class EAnimationHitReactionSource : int8_t {
        FireArm = 0,
        MeleeImpact = 1,
        Throwable = 2,
        Explosion = 3,
    };

    // Size: 0x1
    enum class EAnimationHumanoidHitStrength : int8_t {
        Light = 0,
        Heavy = 1,
    };

    // Size: 0x1
    enum class EAnimationItemState : int8_t {
        Unequipped = 0,
        Equipped = 1,
        Equipping = 2,
        Unequipping = 3,
        Reloading = 4,
        Chambering = 5,
        Unused2 = 6,
        PickingUp = 7,
        Dropping = 8,
    };

    // Size: 0x1
    enum class EAnimationLocomotionAgility : int8_t {
        None = 0,
        WallLedgeHang = 1,
        OpenLedgeHang = 2,
        LedgeSidle = 3,
        Ladder = 4,
        Drainpipe = 5,
        ZipLine = 6,
    };

    // Size: 0x1
    enum class EAnimationLocomotionApproach : int8_t {
        None = 0,
        Approaching = 1,
    };

    // Size: 0x1
    enum class EAnimationLocomotionContext : int8_t {
        Formal = 0,
        Casual = 1,
        Combat = 2,
    };

    // Size: 0x1
    enum class EAnimationLocomotionCover : int8_t {
        None = 0,
        RegularCover = 1,
        MovableCover = 2,
    };

    // Size: 0x1
    enum class EAnimationLocomotionDash : int8_t {
        None = 0,
        Dashing = 1,
    };

    // Size: 0x1
    enum class EAnimationLocomotionDisoriented : int8_t {
        None = 0,
        Disoriented = 1,
    };

    // Size: 0x1
    enum class EAnimationLocomotionEquippedItem : int8_t {
        None = 0,
        Firearm = 1,
        Throwable = 2,
    };

    // Size: 0x1
    enum class EAnimationLocomotionFacing : int8_t {
        TrajectoryForward = 0,
        Strafing = 1,
    };

    // Size: 0x1
    enum class EAnimationLocomotionGameplayArea : int8_t {
        Social = 0,
        SoftTrespassing = 1,
        Trespassing = 2,
        LicenseToKill = 3,
    };

    // Size: 0x1
    enum class EAnimationLocomotionSpeed : int8_t {
        Slow = 0,
        Medium = 1,
        Fast = 2,
    };

    // Size: 0x1
    enum class EAnimationLocomotionStance : int8_t {
        Standing = 0,
        Crouching = 1,
    };

    // Size: 0x1
    enum class EAnimationLocomotionStateType : int8_t {
        Speed = 0,
        Style = 1,
        Stance = 2,
        Facing = 3,
        EquippedItem = 4,
        Cover = 5,
        Dash = 6,
        Approach = 7,
        Agility = 8,
        GameplayArea = 9,
        PlayerStyle = 10,
        Combat = 11,
        Disoriented = 12,
    };

    // Size: 0x1
    enum class EAnimationLocomotionStyle : int8_t {
        Casual = 0,
        Tense = 1,
        CloseCombat = 2,
        AmbientSearch_NPC = 3,
        RangedCombat = 4,
        Panic = 5,
    };

    // Size: 0x1
    enum class EAnimationParryStep : int8_t {
        None = 0,
        Attacking = 1,
        Impacting = 2,
        Failing = 3,
        Staggered = 4,
    };

    // Size: 0x4
    enum class EAnimationPayloadEvent : int32_t {
        OnFirearmHitReaction = 0,
        OnItemEquip = 1,
    };

    // Size: 0x1
    enum class EAnimationStumbleImpactStrength : int8_t {
        Light = 0,
        Heavy = 1,
    };

    // Size: 0x1
    enum class EApproachMovementFinishPolicy : int8_t {
        OnlyOwnSource = 0,
        AnySource = 1,
    };

    // Size: 0x1
    enum class EApproachPathFailReason : int8_t {
        None = 0,
        StartLocationNoNavmesh = 1,
        EndLocationNoNavmesh = 2,
        PathNotFound = 3,
        PathTooShort = 4,
    };

    // Size: 0x1
    enum class EArchetypeRoleAvailabilityType : int8_t {
        Default = 0,
        Always = 1,
        Never = 2,
    };

    // Size: 0x4
    enum class EAttackFromAboveState : int32_t {
        None = 0,
        Jump = 1,
        Impact = 2,
    };

    // Size: 0x4
    enum class EAttackFromAboveType : int32_t {
        Invalid = -1,
        Falling = 0,
        Edge = 1,
        Railing = 2,
        Hang = 3,
        Sidle = 4,
        Zipline = 5,
    };

    // Size: 0x1
    enum class EBasCharacterSourceType : int8_t {
        Self = 0,
        TargetEnemy = 1,
        AnotherNPC = 2,
        None = 3,
    };

    // Size: 0x1
    enum class EBasValueComparisonOperationType : int8_t {
        Equal = 0,
        NotEqual = 1,
        Greater = 2,
        GreaterOrEqual = 3,
        Less = 4,
        LessOrEqual = 5,
    };

    // Size: 0x1
    enum class EBlastFieldComponentType : int8_t {
        WakeUpAndKinematic = 0,
        PhysicsImpulse = 1,
        Destruction = 2,
        ClothImpulse = 3,
        GameplayEvent = 4,
    };

    // Size: 0x1
    enum class EBlastFieldDestructionType : int8_t {
        Impact = 0,
        Explosion = 1,
    };

    // Size: 0x1
    enum class EBlastFieldForceDirectionType : int8_t {
        BlastFieldTransform = 0,
        CustomValue = 1,
    };

    // Size: 0x1
    enum class EBlastFieldImpulseForceApplicationType : int8_t {
        CenterOfMass = 0,
        BoundingBoxCenter = 1,
        ProjectionToBoundingBox = 2,
    };

    // Size: 0x1
    enum class EBlastFieldSensorShapeType : int8_t {
        Sphere = 0,
        Box = 1,
    };

    // Size: 0x1
    enum class EBlastFieldSourceType : int8_t {
        Unknown = 0,
        CoreLogic = 1,
        Projectiles = 2,
        Stumbles = 3,
        PairedActions = 4,
        MeleeAttacks = 5,
    };

    // Size: 0x4
    enum class EBossEncounterPhase : int32_t {
        PhaseOne = 0,
        PhaseTwo = 1,
        PhaseThree = 2,
        PhaseFour = 3,
    };

    // Size: 0x1
    enum class EBossType : int8_t {
        Assassin = 0,
        Jaws = 1,
        Damien = 2,
        ExoDamien = 3,
    };

    // Size: 0x4
    enum class ECameraMeleeTargetType : int32_t {
        CurrentTarget = 0,
        FinisherTarget = 1,
        CurrentTargetMidpoint = 2,
    };

    // Size: 0x4
    enum class ECharacterProviderCharacterType : int32_t {
        NPC = 0,
        Player = 1,
        VoiceOver = 2,
    };

    // Size: 0x1
    enum class ECinematicTransitionApproachTargetType : int8_t {
        AnimationTrack = 0,
        TrajectoryTrack = 1,
    };

    // Size: 0x4
    enum class ECinematicsDebugSequenceTargetEntityType : int32_t {
        HumanoidSequenceController = 0,
        VehicleSequenceController = 1,
        Others = 2,
    };

    // Size: 0x4
    enum class ECinematicsDebugSequenceTrackType : int32_t {
        AnimationClip = 0,
        AnimationNetworkClip = 1,
        AnimationNetworkParameter = 2,
        AudioSequence = 3,
        CameraLensPreset = 4,
        CameraSequenceClip = 5,
        EntityProperty = 6,
        HumanoidControl = 7,
        VehicleControl = 8,
        InjectedAnimationNetwork = 9,
        Trajectory = 10,
        Undefined = 11,
    };

    // Size: 0x1
    enum class EClimbAction : int8_t {
        None = -1,
        LedgeHangMount = 0,
        LedgeHangDismount = 1,
        LedgeHangToLedgeHang = 2,
        LedgeHangToLedgeHangSharpTurnOut = 3,
        LedgeHangToLedgeHangSharpTurnIn = 4,
        LedgeHangToLedgeHangSoftTurn = 5,
        LedgeHangToSidle = 6,
        LedgeHangToLadder = 7,
        LedgeHangToDrainpipe = 8,
        LedgeHangToRailing = 9,
        LedgeHangToWindow = 10,
        SidleMount = 11,
        SidleDismount = 12,
        SidleToLedgeHang = 13,
        SidleToSidle = 14,
        SidleToSidleSwap = 15,
        SidleToSidleSharpTurnOut = 16,
        SidleToSidleSharpTurnIn = 17,
        SidleToSidleSoftTurn = 18,
        SidleToLadder = 19,
        SidleToDrainpipe = 20,
        SidleToRailingPerpendicular = 21,
        SidleToRailingCollinear = 22,
        SidleToWindowPerpendicular = 23,
        SidleToWindowCollinear = 24,
        LadderMount = 25,
        LadderDismount = 26,
        LadderToLedgeHang = 27,
        LadderToSidle = 28,
        LadderToLadder = 29,
        LadderToDrainpipe = 30,
        DrainpipeMount = 31,
        DrainpipeDismount = 32,
        DrainpipeToLedgeHang = 33,
        DrainpipeToSidle = 34,
        DrainpipeToLadder = 35,
        DrainpipeToDrainpipe = 36,
        RailingToLedgeHang = 37,
        RailingToSidle = 38,
        WindowToLedgeHang = 39,
        WindowToSidle = 40,
        EdgeStepback = 41,
    };

    // Size: 0x1
    enum class EClimbDirection : int8_t {
        None = -1,
        Up = 0,
        Down = 1,
        Right = 2,
        Left = 3,
    };

    // Size: 0x1
    enum class EClimbObstacleSide : int8_t {
        None = 0,
        Right = 1,
        Left = 2,
    };

    // Size: 0x1
    enum class EClimbingState : int8_t {
        NotClimbing = 0,
        CurrentlyClimbing = 1,
        EnteringClimbFromBelow = 2,
        EnteringClimbFromAbove = 3,
        EnteringClimbFromLeft = 4,
        EnteringClimbFromRight = 5,
        LeavingClimbToBelow = 6,
        LeavingClimbToAbove = 7,
        LeavingClimbToLeft = 8,
        LeavingClimbToRight = 9,
    };

    // Size: 0x4
    enum class ECloseCombatEventType : int32_t {
        LightAttack = 0,
        HeavyAttack = 1,
        Parry = 2,
        Grab = 3,
        PairedAction = 4,
        Throw = 5,
        Sidestep = 6,
        VaultAttack = 7,
        Block = 8,
        Invalid = -1,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionAvailability : int32_t {
        Always = 0,
        Mandatory = 1,
        Forbidden = 2,
    };

    // Size: 0x1
    enum class ECloseCombatPairedActionBlastFieldTriggerEvent : int8_t {
        AttackImpact = 0,
        DamageVictim = 1,
    };

    // Size: 0x1
    enum class ECloseCombatPairedActionBlastFieldTriggerOrientation : int8_t {
        CharacterForward = 0,
        BoneVelocity = 1,
        BoneOrientation = 2,
        ContextualTarget = 3,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionCheckResult : int32_t {
        NotEvaluated = 0,
        Rejected_InstigatorIsCombatRushingAndActionIsNotAvailable = 1,
        Rejected_InstigatorIsNotCombatRushingAndActionIsNotAvailable = 2,
        Rejected_VictimIsFurtherThanMaxDistance = 3,
        Rejected_RequiredContextualTargetWasNotFound = 4,
        Rejected_ContextualTargetIsFurtherAwayThanBestOne = 5,
        Rejected_NoClearPathFromInstigatorToSafeArea = 6,
        Rejected_NoClearPathFromVictimToSafeArea = 7,
        Rejected_InstigatorDoesntHonorRequiredDirectionToVictim = 8,
        Rejected_VictimIsNotOnFloor = 9,
        Rejected_VictimOnFloor = 10,
        Rejected_VictimIsNotStaggerStunned = 11,
        Rejected_VictimIsStaggerStunned = 12,
        Rejected_VictimIsNotParryStunned = 13,
        Rejected_VictimIsParryStunned = 14,
        Rejected_InstigatorIsNotOnStairs = 15,
        Rejected_InstigatorIsOnStairs = 16,
        Rejected_VictimIsNotOnStairs = 17,
        Rejected_VictimIsOnStairs = 18,
        Rejected_InstigatorKeywordRequirementsNotHonored = 19,
        Accepted_ValidCandidate = 20,
        Selected = 21,
    };

    // Size: 0x1
    enum class ECloseCombatPairedActionContextualTargetCheckResult : int8_t {
        NotEvaluated = 0,
        Rejected_InstigatorIsNotPlayer = 1,
        Rejected_MissingVictimsCache = 2,
        Rejected_InvalidContextualTargetType = 3,
        Rejected_MissingSelectedItemData = 4,
        Rejected_NotInStairs = 5,
        Rejected_NotInSlopeStairs = 6,
        Rejected_InvalidConfigVictimDirectionToTarget = 7,
        Rejected_ExpectedTargetNotFound = 8,
        Rejected_WallIsInvalid = 9,
        Rejected_WallIsLowCover = 10,
        Rejected_WallIsHighWall = 11,
        Rejected_WallWidthIsTooLow = 12,
        Rejected_BullrusheableWallIgnored = 13,
        Rejected_InstigatorTooCloseToVictim = 14,
        Rejected_VictimTooFarFromTarget = 15,
        Rejected_VictimTooFarFromTargetOnLateralDisplacement = 16,
        Rejected_AngleBetweenInstigatorForwardAndDirectionToTarget = 17,
        Rejected_AngleBetweenVictimForwardAndDirectionToTarget = 18,
        Rejected_AngleBetweenInstigatorVictimVectorAndTargetOrientation = 19,
        Rejected_TargetOutsideSearchDirection = 20,
        Accepted_ValidTarget = 21,
        Accepted_BestTarget = 22,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionContextualTargetPriority : int32_t {
        CloserToVictimByDistance = 0,
        CloserToSearchDirectionByAngle = 1,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionContextualTargetRestriction : int32_t {
        PreferCurrentTargetIfSet = 0,
        TryUseOnlyCurrentTargetIfSet = 1,
        IgnoreCurrentTarget = 2,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionContextualTargetType : int32_t {
        None = 0,
        Wall = 1,
        LowCover = 2,
        Item = 3,
        Guide = 4,
        OffmetricObstacle = 5,
        Stairs = 6,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionPhase : int32_t {
        Preparing = 0,
        Executing = 1,
        Count = 2,
    };

    // Size: 0x4
    enum class ECloseCombatPairedActionType : int32_t {
        Finisher_Melee = 0,
        SilentTakedown = 1,
        Finisher_Armed = 2,
        Finisher_TaserTakedown = 3,
        ContextualFinisher_LowCover = 4,
        ContextualFinisher_Wall = 5,
        ContextualFinisher_Item = 6,
        Finisher_Disarm = 7,
        UnusedType_1 = 8,
        Finisher_Custom = 9,
        Contextual_MeleeAttack = 10,
        FakeSurrender = 11,
        DisarmAttack = 12,
        GrabRushOffTheLedge = 13,
        ContextualTargetFinisher = 14,
        Contextual_ThrowAttack = 15,
        BrutalAttack = 16,
    };

    // Size: 0x4
    enum class ECloseCombatReactionCounterType : int32_t {
        MeleeCounter = 0,
        Grab = 1,
        MeleeCombo = 2,
    };

    // Size: 0x1
    enum class ECloseCombatStance : int8_t {
        Default = 0,
        Parry = 1,
        Aggressive = 2,
        NotInCloseCombat = 3,
        Relaxed = 4,
    };

    // Size: 0x4
    enum class ECombatRushTargetCriteria : int32_t {
        NoValidTarget = 0,
        NoMeleeTargetsAvailable = 1,
        OngoingCombatRushTarget = 2,
        InputWinner = 3,
        ConfrontationTarget = 4,
        OnScreen = 5,
        InRange = 6,
        ClosestTarget = 7,
        TargetIsImmune = 8,
        TargetUnreachable = 9,
    };

    // Size: 0x2
    enum class ECombatRushType : int16_t {
        Strike = 0,
        SilentTakedown = 1,
        RushAndDisarm = 2,
        Finisher = 3,
        ContextualAttack = 4,
        DisarmAttack = 5,
        Grab = 6,
        RushToAttackFromAbove = 7,
    };

    // Size: 0x1
    enum class EConfrontationEndReason : int8_t {
        Failed = 0,
        FakeSurrender = 1,
        Gadget = 2,
        Bluff = 3,
    };

    // Size: 0x1
    enum class EConfrontationUpperbodyAnimCategoryType : int8_t {
        None = -1,
        Apologetic = 0,
        Confident = 1,
        Nonchalant = 2,
    };

    // Size: 0x1
    enum class EConfrontationUpperbodyAnimType : int8_t {
        None = 0,
        ActiveBluffEnabled = 1,
        ActiveBluffDone = 2,
        ConfrontationBluffEnabled = 3,
        ConfrontationBluffDone = 4,
    };

    // Size: 0x1
    enum class EContextualJumpLandingSurfaceType : int8_t {
        OverWall = 0,
        LedgeOverhang = 1,
        OnLedgeHang = 2,
    };

    // Size: 0x1
    enum class EContextualJumpLandingType : int8_t {
        Regular = 0,
        Sketchy = 1,
    };

    // Size: 0x1
    enum class EContextualJumpType : int8_t {
        None = 0,
        Jump = 1,
        JumpDown = 2,
        JumpUpAndClimb = 3,
    };

    // Size: 0x4
    enum class EContextualWallType : int32_t {
        Wall = 0,
        LowCover = 1,
        Invalid = 2,
    };

    // Size: 0x4
    enum class EConversationChoiceResult : int32_t {
        Option1 = 0,
        Option2 = 1,
        Option3 = 2,
        Option4 = 3,
        TimedOut = 4,
        Invalid = 5,
    };

    // Size: 0x4
    enum class EConversationChoiceType : int32_t {
        Flavor = 0,
        Progression = 1,
        Leave = 2,
    };

    // Size: 0x4
    enum class ECornerType : int32_t {
        Concave = 0,
        Convex = 1,
        None = 2,
    };

    // Size: 0x1
    enum class ECoverCorner : int8_t {
        LeftCorner = 0,
        RightCorner = 1,
        NoCorner = 2,
    };

    // Size: 0x1
    enum class ECoverCornerTurnAnimState : int8_t {
        NoTurning = 0,
        Turning = 1,
    };

    // Size: 0x1
    enum class ECoverEnterSource : int8_t {
        None = 0,
        WalkIn = 1,
        TransitionToNewAdjacentCover = 2,
        TransitionToUpdatedCover = 3,
        Dash = 4,
        CoverCornerTurn = 5,
        Cinematics = 6,
        AimTransition = 7,
        CoverCornerTransition = 8,
    };

    // Size: 0x1
    enum class ECoverExitSource : int8_t {
        None = 0,
        WalkOut = 1,
        DashEndedOutFarFromCover = 2,
        StandupInLowCover = 3,
        Agility = 4,
        Confrontation = 5,
        MarkupChangedAndCoverNotAllowed = 6,
        Acceleration = 7,
        AimTransition = 8,
        CombatRush = 9,
        CoverSystemDisabled = 10,
        SplineInvalidPoint = 11,
        SplineWalkOut = 12,
        CoverCorner = 13,
        UntaggedCoverCorner = 14,
        Interaction = 15,
        FreeDash = 16,
        Staggered = 17,
        CloseCombatAction = 18,
        CloseCombatHitReaction = 19,
        AlignmentMovement = 20,
        DestroyedMarkup = 21,
    };

    // Size: 0x1
    enum class ECoverFacingDir : int8_t {
        Left = 0,
        Right = 1,
    };

    // Size: 0x1
    enum class ECoverHeight : int8_t {
        None = 0,
        LowCover = 1,
        HighCover = 2,
    };

    // Size: 0x1
    enum class ECoverLeanAimingTransitionType : int8_t {
        None = 0,
        ToOutOfCover = 1,
        ToCover = 2,
    };

    // Size: 0x1
    enum class ECoverState : int8_t {
        None = 0,
        GoingOutOfCover = 1,
        GoingIntoCover = 2,
        BehindCover = 3,
        OutsideCover = 4,
    };

    // Size: 0x1
    enum class ECoverStayInLineCornerConstraint : int8_t {
        NoCornerConstraints = 0,
        StopAtTaggedCorners = 1,
    };

    // Size: 0x1
    enum class ECoverStayInLineInputConstraint : int8_t {
        NoInputConstraints = 0,
        OnlyInputToRightEnd = 1,
        OnlyInputToLeftEnd = 2,
    };

    // Size: 0x4
    enum class ECrouchingSubsate : int32_t {
        eCS_NotCrouching = 0,
        eCS_Idle = 1,
        eCS_Aiming = 2,
        eCS_Moving = 3,
        eCS_MovingAndAiming = 4,
    };

    // Size: 0x1
    enum class EDamageType : int8_t {
        Projectile = 1,
        CloseCombat = 2,
        Fall = 3,
        Script = 4,
        Stumble = 5,
        ThrownItem = 6,
        CoverTakedown = 7,
        AttackFromAbove = 8,
        VaultDisarm = 9,
        VaultFinisher = 10,
        Explosion = 11,
        CrushedByObject = 12,
        Vehicle = 13,
        Electric = 14,
        Stealth = 15,
        Environmental = 16,
        RailingDeath = 17,
        OverTheLedge = 18,
        FlashMine = 19,
    };

    // Size: 0x4
    enum class EDangerAwarenessType : int32_t {
        Hazard = 0,
        PotentialThreat = 1,
        NonLethalCombat = 2,
        LethalCombat = 3,
    };

    // Size: 0x4
    enum class EDangerLevel : int32_t {
        EnemiesUnaware = 0,
        EnemiesSearching = 1,
        InCombat = 2,
    };

    // Size: 0x4
    enum class EDashDataSource : int32_t {
        Player = 0,
        GpwInput = 1,
    };

    // Size: 0x4
    enum class EDashSearchBoxValidationResult : int32_t {
        Outside = 0,
        InsideFrontBox = 1,
        InsideBackBox = 2,
    };

    // Size: 0x4
    enum class EDashTargetCornerType : int32_t {
        None = 0,
        Left = 1,
        Right = 2,
    };

    // Size: 0x4
    enum class EDashTargetType : int32_t {
        None = 0,
        LowCover = 1,
        Wall = 2,
    };

    // Size: 0x4
    enum class EDashTargetingSearchConfigType : int32_t {
        Default = 0,
        ParallelCoverSwap = 1,
        NoInput = 2,
    };

    // Size: 0x4
    enum class EDashType : int32_t {
        None = 0,
        ToCover = 1,
        CoverSwap = 2,
        Free = 3,
    };

    // Size: 0x4
    enum class EDashingType : int32_t {
        None = 0,
        DashToCover = 1,
        DashToCoverSlide = 2,
        Free = 3,
    };

    // Size: 0x1
    enum class EDeathVariationType : int8_t {
        Default = 0,
        StumbleDeath_WallImpact = 1,
        StumbleDeath_LowcoverImpact = 2,
        StumbleDeath_Railing = 3,
        StumbleDeath_LowCoverRoll = 4,
    };

    // Size: 0x1
    enum class EDebugAgilityActionOpportunityFailReason : int8_t {
        None = 0,
        HumanoidStateRequirement = 1,
        Allowance = 2,
        Requirements = 3,
        AllowedOnCandidates = 4,
        ConnectionRequirements = 5,
        OpportunityConditions = 6,
        Count = 7,
    };

    // Size: 0x1
    enum class EDebugAgilityActionOpportunityPendingReason : int8_t {
        None = 0,
        TriggerConditions = 1,
        Interruption = 2,
        GadgetView = 3,
        Count = 4,
    };

    // Size: 0x1
    enum class EDebugAgilityActionOpportunityState : int8_t {
        None = 0,
        Failed = 1,
        Restricted = 2,
        Pending = 3,
        Performable = 4,
        Performing = 5,
        Count = 6,
    };

    // Size: 0x4
    enum class EDebugNpcBehaviorType : int32_t {
        None = 0,
        FightFromCover = 1,
        FightFromCurrentPosition = 2,
        ThrowGrenade = 3,
    };

    // Size: 0x4
    enum class EDebugNpcCombatLocomotionType : int32_t {
        Default = 0,
        Run = 1,
        StrafeWalk = 2,
    };

    // Size: 0x1
    enum class EDefenseAction : int8_t {
        None = 0,
        Activate = 1,
        Deactivate = 2,
    };

    // Size: 0x4
    enum class EDefenseType : int32_t {
        MeleeAttack = 0,
        Grab = 1,
        RedirectThrow = 2,
        StumbleHumanoidImpact = 3,
        Explosion = 4,
    };

    // Size: 0x1
    enum class EDetainState : int8_t {
        None = 0,
        Starting = 1,
        Idle = 2,
        Struggle = 3,
        Attack = 4,
        HitReaction = 5,
        BreakingFree = 6,
        Failed = 7,
    };

    // Size: 0x1
    enum class EDialogueActivityLevel : int8_t {
        PlayEssential = 0,
        CombatGameplay = 1,
        CombatAmbient = 2,
        TensionGameplay = 3,
        TensionAmbient = 4,
        CalmGameplay = 5,
        CalmAmbient = 6,
        Vocalization = -6,
        Ignore = -1,
    };

    // Size: 0x2
    enum class EDialogueFactOp : int16_t {
        Add = 0,
        Subtract = 1,
        Set = 2,
    };

    // Size: 0x1
    enum class EDisorientationSourceType : int8_t {
        PoisonDart = 0,
        GasLeak = 1,
        MigraineDevice = 2,
        Blinded = 3,
    };

    // Size: 0x4
    enum class EDistractionSource : int32_t {
        Default = 0,
        Whistle = 1,
        DartGun = 2,
        SilentTakedown = 3,
        Laptop = 4,
        Radio = 5,
        AC = 6,
        DoorKick = 7,
        Footsteps = 8,
        GardenHose = 9,
        Gadget_Smoke = 10,
        Gadget_FlashMine = 11,
        Gadget_FlashMineHumanTarget = 12,
        ElectrifiedHuman = 13,
        DisorientedGeneric = 14,
        PoisonDart = 15,
        Blinded = 16,
        Shockwave = 17,
        IncapacitatedBody = 18,
        Explosions = 27,
        BullectImpact = 28,
        Touch = 29,
        SecurityCamera = 30,
        Fixable = 31,
        SpotDisoriented = 32,
    };

    // Size: 0x4
    enum class EDoorInitialState : int32_t {
        Closed = 0,
        OpenForward = 1,
        OpenBack = 2,
        Locked = 3,
        Sabotaged = 4,
    };

    // Size: 0x1
    enum class EEarlyOutCapabilityIssuer : int8_t {
        Unknown = 0,
        Dash = 1,
        MeleeAttack = 2,
        FakeSurrender = 3,
        ContextualJump = 4,
        Landing = 5,
        Vaulting = 6,
        Scaling = 7,
        AgilityAction = 8,
        GenericAnimDrivenAction = 9,
        TurningAtCoverCorner = 10,
        UseDoor = 11,
        PairedActionInstigator = 12,
        PairedActionVictim = 13,
        PickUp = 14,
        EnterLadder = 15,
        LeaveLadder = 16,
        Pickpocket = 17,
        Grab = 18,
        Parry = 19,
        ItemThrow = 20,
        GadgetActivation = 21,
        CombatRush = 22,
        FirearmReload = 23,
        Stagger = 24,
        Sidestep = 25,
        Block = 26,
        CoverTakedown = 27,
        AttackFromAbove = 28,
    };

    // Size: 0x1
    enum class EEavesdropUpperbodyAnimType : int8_t {
        None = 0,
        PlayerStillStarted = 1,
        PlayerStillEnded = 2,
    };

    // Size: 0x4
    enum class EEnableDisableNPCAbilityType : int32_t {
        Vision = 0,
        Hearing = 1,
        Agility = 2,
        DistractionReaction = 3,
        PrivateSpace = 4,
        Crouch = 5,
    };

    // Size: 0x1
    enum class EEntryLocationType : int8_t {
        Default = 0,
        Archway = 1,
        StairsUp = 2,
        StairsDown = 3,
    };

    // Size: 0x4
    enum class EEnvironmentMarkupAgilityType : int32_t {
        None = 0,
        Vaultable = 1,
        Scalable = 2,
        Jump = 3,
        GenericAnimationDrivenAction = 4,
    };

    // Size: 0x4
    enum class EEnvironmentMarkupCoverType : int32_t {
        None = 0,
        Low = 1,
        High = 2,
    };

    // Size: 0x4
    enum class EEnvironmentMarkupLevelDesignBool : int32_t {
        eEMLDB_Automatic = 0,
        eEMLDB_True = 1,
        eEMLDB_False = 2,
    };

    // Size: 0x4
    enum class EEnvironmentalLockState : int32_t {
        Unlocked = 0,
        Sit_100cm = 1,
        Sit_020cm = 2,
        Sit_000cm = 3,
        Sit_Stool_080cm = 4,
        Sit_Chair = 5,
        Sit_Couch = 6,
        Lean_120cm = 7,
        Lean_100cm = 8,
        Lean_075cm = 9,
        Lean_Wall = 10,
        Stand = 11,
    };

    // Size: 0x1
    enum class EEvaluatorMode : int8_t {
        Add = 0,
        Multiply = 1,
        Multiply_SingleEval = 2,
    };

    // Size: 0x1
    enum class EFirearmDamageMultiplierFilter : int8_t {
        Always = 0,
        BodyPartWeakpoint = 1,
    };

    // Size: 0x1
    enum class EFirearmHumanoidHitState : int8_t {
        NoFlinch = 0,
        Flinch = 1,
        Stumble = 2,
        HeavyHit = 3,
        Stagger = 4,
        AnimationFullBody = 5,
    };

    // Size: 0x4
    enum class EFirearmPosture : int32_t {
        Lowered = 0,
        Primed = 1,
        Raised = 2,
        AboveHead = 3,
        BlindFiringLeft = 4,
        BlindFiringRight = 5,
        LeaningLeft = 6,
        LeaningRight = 7,
    };

    // Size: 0x4
    enum class EFootstepEventType : int32_t {
        Automatic = 0,
        Normal = 1,
        Stair = 2,
        Jump = 3,
        Land = 4,
        Slide = 5,
        Start = 6,
        Stop = 7,
        Turn = 8,
        Count = 9,
    };

    // Size: 0x1
    enum class EFootstepType : int8_t {
        Left = 0,
        Right = 1,
        Any = 2,
    };

    // Size: 0x4
    enum class EGadgetActivationSlot : int32_t {
        Slot1 = 0,
        Slot2 = 1,
        Slot3 = 2,
        Slot4 = 3,
    };

    // Size: 0x1
    enum class EGadgetDisabledFlag : int8_t {
        GadgetNotAssigned = 0,
        PlayerCantUseGadgets = 1,
        InCooldown = 2,
        CantBeUsedInArea = 3,
        UnusedEntry_0 = 4,
        NotEnoughResources = 5,
        InvalidTarget = 6,
        TargetOutOfRange = 7,
        Count = 8,
    };

    // Size: 0x1
    enum class EGadgetState : int8_t {
        None = 0,
        TapActivate = 1,
        HoldActivate = 2,
    };

    // Size: 0x1
    enum class EGadgetTargetInteractionType : int8_t {
        Tap = 0,
        Hold = 1,
        TapAndHold = 2,
    };

    // Size: 0x4
    enum class EGameplayAreaLocomotionContext : int32_t {
        Formal = 0,
        Casual = 1,
        Combat = 2,
    };

    // Size: 0x4
    enum class EGameplayAreaType : int32_t {
        Social = 0,
        SoftTrespassing = 1,
        Trespassing = 2,
        LicenseToKill = 3,
    };

    // Size: 0x4
    enum class EGoalPriorities : int32_t {
        HIGHEST = 0,
        STAGGER = 1,
        CONFRONTATION_OPT_IN = 2,
        LURE = 3,
        BLUFF = 4,
        ATTACK_FROM_ABOVE = 5,
        VAULT = 6,
        CONTEXTUAL_JUMP = 7,
        TURN_AT_COVER_CORNER = 8,
        AGILITY_ACTION = 9,
        SCALE = 10,
        DASH = 11,
        GENERIC_ANIM_DRIVEN_ACTION = 12,
        USE_DOOR = 13,
        GROUND_PATH_LOCOMOTION = 14,
        COMPLYING = 15,
        DROP_DOWN = 16,
        COMBATRUSH = 17,
        PROJECTILE_SIDESTEP = 18,
        ACTIVATE_GADGET = 19,
        MELEE_FINISHER = 20,
        PARRY = 21,
        PICKPOCKET = 22,
        GRAB = 23,
        BLOCK = 24,
        SIDESTEP = 25,
        COVER_TAKEDOWN = 26,
        CONTEXTUAL_ITEM_THROW = 27,
        ITEM_THROW = 28,
        JUMPING_AWAY_HAZARD = 29,
        MELEE_ATTACK = 30,
        COVER_TRANSITION = 31,
        ENTER_LADDER_CLIMB = 32,
        LEAVE_LADDER_CLIMB = 33,
        CROUCH = 34,
        UNCROUCH = 35,
        SET_FIREARM_POSTURE = 36,
        SET_THROWABLE_POSTURE = 37,
        CHANGE_CLOSECOMBAT_STANCE = 38,
        SET_AIM_TARGET = 39,
        START_STRAFING = 40,
        STOP_STRAFING = 41,
        START_ACCELERATING = 42,
        STOP_ACCELERATING = 43,
        GADGET_VIEW = 44,
        PICKUP_ITEM = 45,
        INTERACT = 46,
        ANIMATION_ACT = 47,
        LOOK_AT = 48,
        UNEQUIP_ITEM = 49,
        EQUIP_ITEM = 50,
        RELOAD_FIREARM = 51,
        FAKE_SURRENDER = 52,
        WHISTLE = 53,
        CAMERA_FOCUS = 54,
        PRIVATE_SPACE_INVADED_REACTION = 55,
        AGENCY_FOCUS = 56,
        LOWEST = 57,
        COUNT = 58,
    };

    // Size: 0x4
    enum class EGrabRushOffTheLedgeGroundPositionResult : int32_t {
        Invalid_NoGround = 0,
        Invalid_OutOfReach = 1,
        Invalid_NoNavMeshPoint = 2,
        Valid = 3,
    };

    // Size: 0x4
    enum class EGrabRushOffTheLedgeType : int32_t {
        Throw = 0,
        Jump = 1,
        Count = 2,
    };

    // Size: 0x4
    enum class EGrabStepFlags : int32_t {
        Taunting = 1,
        FailingGrab = 2,
        PreparingGrab = 4,
        Attacking = 8,
        Throwing = 16,
        PostThrowing = 32,
        BreakingFree = 64,
        Rushing = 128,
    };

    // Size: 0x4
    enum class EGrabThrowType : int32_t {
        None = 0,
        BasicThrow = 1,
        ThrowFromAttack = 2,
    };

    // Size: 0x4
    enum class EGrabVictimStatus : int32_t {
        Invalid = 0,
        Valid_PendingSuccessCheck = 1,
        Valid_FailedGrab = 2,
        Valid_SuccessfulGrab = 3,
    };

    // Size: 0x4
    enum class EGrabbingStep : int32_t {
        Taunting = 0,
        FailingGrab = 1,
        PreparingGrab = 2,
        Attacking = 3,
        Throwing = 4,
        PostThrowing = 5,
        BreakingFree = 6,
        Rushing = 7,
    };

    // Size: 0x4
    enum class EGuidanceLocomotionThresholdReachedHandlingType : int32_t {
        Disabled = 0,
        Systemic = 1,
    };

    // Size: 0x1
    enum class EGuideTakedownType : int8_t {
        None = 0,
        OpenLedgeHang = 1,
        OpenLedgeHangWithRailing = 2,
        WallLedgeHang = 3,
        WallLedgeHangWithRailing = 4,
        SidleWithRailingRight = 5,
        SidleWithRailingLeft = 6,
    };

    // Size: 0x4
    enum class EHazardSource : int32_t {
        Default = 0,
        Grenade = 1,
        Chandelier = 2,
        Spotlight = 3,
        Lift = 4,
        Pipe = 5,
        AirConditioner = 6,
        ConcreteBag = 7,
        SmokeBomb = 8,
    };

    // Size: 0x4
    enum class EHealingSource : int32_t {
        HealthRegen = 0,
        CoreLogic = 1,
    };

    // Size: 0x1
    enum class EHitReactionDefenseAction : int8_t {
        Ignore = 0,
        Enabled = 1,
        Disabled = 2,
    };

    // Size: 0x1
    enum class EHitReactionDisabledSkill : int8_t {
        Aim = 0,
        Shoot = 1,
        Sprint = 2,
        MeleeAttack = 3,
        Count = 4,
    };

    // Size: 0x1
    enum class EHitReactionSource : int8_t {
        FireArm = 0,
        MeleeImpact = 1,
        Throwable = 2,
        Explosion = 3,
    };

    // Size: 0x4
    enum class EHumanMovementCategory : int32_t {
        eHMCa_Ambient = 0,
        eHMCa_TacticalSearch = 1,
        eHMCa_CloseCombat = 2,
        eHMCa_RangedCombat = 3,
        eHMCa_AmbientSearch = 4,
    };

    // Size: 0x4
    enum class EHumanMovementContext : int32_t {
        eHMC_Walk = 0,
        eHMC_Run = 1,
        eHMC_Sprint = 2,
    };

    // Size: 0x4
    enum class EHumanoidAudioLocomotionContext : int32_t {
        Social = 0,
        Trespassing = 1,
        SoftTrespassing = 2,
        CloseCombat = 3,
        Aiming = 4,
    };

    // Size: 0x4
    enum class EHumanoidBodyPart : int32_t {
        None = 0,
        Torso = 1,
        Head = 2,
        LeftHand = 3,
        LeftLowerArm = 4,
        LeftUpperArm = 5,
        LeftShoulder = 6,
        LeftFoot = 7,
        LeftLowerLeg = 8,
        LeftUpperLeg = 9,
        RightHand = 10,
        RightLowerArm = 11,
        RightUpperArm = 12,
        RightShoulder = 13,
        RightFoot = 14,
        RightLowerLeg = 15,
        RightUpperLeg = 16,
        Weapon = 17,
    };

    // Size: 0x1
    enum class EHumanoidBoneCollisionPresetType : int8_t {
        Default = 0,
        Agility = 1,
    };

    // Size: 0x1
    enum class EHumanoidBoneTransformFlag : int8_t {
        UseCharacterRotation = 0,
        UseCharacterZ = 1,
    };

    // Size: 0x1
    enum class EHumanoidCapsuleDataType : int8_t {
        Stand = 0,
        Crouch = 1,
        Vault = 2,
        SqueezeThrough = 3,
        LedgeHang = 4,
        Ladder = 5,
    };

    // Size: 0x2
    enum class EHumanoidCollisionDisabledSource : int16_t {
        VehicleAttachment = 0,
        CoverTakedown = 1,
        Death = 2,
        CoreLogic = 3,
        GenericAnimDrivenAction = 4,
        AnimationEvents = 5,
        Unknown = 6,
        Climb = 7,
        Guide = 8,
        AttackFromAbove = 9,
    };

    // Size: 0x1
    enum class EHumanoidCollisionPresetType : int8_t {
        Default = 0,
        Agility = 1,
        VirtualCapsule = 2,
        AnimationAct = 3,
    };

    // Size: 0x1
    enum class EHumanoidDamageType : int8_t {
        Unknown = 0,
        Projectile = 1,
        CloseCombat = 2,
        Fall = 3,
        Script = 4,
        Stumble = 5,
        ThrownItem = 6,
        CoverTakedown = 7,
        AttackFromAbove = 8,
        Explosion = 9,
        VaultDisarm = 10,
        VaultFinisher = 11,
        CrushedByObject = 12,
        Vehicle = 13,
        Electric = 14,
        Stealth = 15,
        Environmental = 16,
        RailingDeath = 17,
        OverTheLedge = 18,
        FlashMine = 19,
    };

    // Size: 0x4
    enum class EHumanoidGoalInterruptReason : int32_t {
        Unknown = 0,
        Solver = 1,
        Issuer = 2,
        NoSkill = 3,
        AutomatedRemoval = 4,
        NoValidController = 5,
        OwnerKilled = 6,
        TargetedAction = 7,
        HitReaction = 8,
        Falling = 9,
        ReferencedIdentifierDestroyed = 10,
        InterceptedByScript = 11,
        MissingRequiredGoal = 12,
        InterruptedDuringEarlyOut = 13,
        Stagger = 14,
    };

    // Size: 0x4
    enum class EHumanoidLocomotionMovementPipeline : int32_t {
        Invalid = 0,
        AnimationDriven = 1,
        AnimationDrivenEnteringLadder = 2,
        AnimationDrivenJumping = 3,
        AnimationDrivenLanding = 4,
        AnimationDrivenScaling = 5,
        AnimationDrivenSequenceAnimDisplacement = 6,
        AnimationDrivenSequenceSpatialDisplacement = 7,
        AnimationDrivenStaggered = 8,
        AnimationDrivenVaulting = 9,
        Falling = 10,
        GameplayAligning = 11,
        IdleWithMotionPlan = 12,
        Idle = 13,
        Jumping = 14,
        OnGuide = 15,
        PathDrivenGroundLocomotion = 16,
        PlayerDrivenGroundLocomotion = 17,
        Sliding = 18,
        SpatialDriven = 19,
        SplineDriven = 20,
        Teleporting = 21,
        TeleportingInVehicleCar = 22,
        TeleportingInVehicleBoat = 23,
    };

    // Size: 0x4
    enum class EHumanoidMovementInputSpeed : int32_t {
        Stationary = 0,
        Slow = 1,
        Fast = 2,
    };

    // Size: 0x1
    enum class EHumanoidOnGuideSpecialization : int8_t {
        None = 0,
        Ledge_Hang = 1,
        Ledge_Sidle = 2,
    };

    // Size: 0x4
    enum class EHumanoidTrespassingStance : int32_t {
        None = 0,
        Confident = 1,
        Vigilant = 2,
    };

    // Size: 0x4
    enum class EInputProbeAdjustmentType : int32_t {
        None = 0,
        FaceTowardsSurface = 1,
        MoveAlongSurface = 2,
        Stop = 3,
    };

    // Size: 0x1
    enum class EInteractionDoorAction : int8_t {
        Open = 0,
        Close = 1,
        Lock = 2,
    };

    // Size: 0x1
    enum class EInteractionDoorHandleSide : int8_t {
        Left = 0,
        Right = 1,
    };

    // Size: 0x1
    enum class EInteractionDoorOpenStyle : int8_t {
        Normal = 0,
        Charge = 1,
        Kick = 2,
    };

    // Size: 0x4
    enum class EInteractionUIAgilityType : int32_t {
        Invalid = 0,
        AttackFromAbove = 1,
        ClimbLadder = 2,
        Climb = 3,
        Vault = 4,
        VaultAttack = 5,
        VaultDropdown = 6,
        VaultUp = 7,
        Jump = 8,
        GenericAnimDrivenAction = 9,
        Takedown = 10,
        Parry = 11,
        Rush = 12,
        Disarm = 13,
        CombatRush = 14,
        CombatRushStrike = 15,
        CombatRushSilentTakedown = 16,
        AgencyBluff = 17,
        AgencyFinisher = 18,
        ZiplineEnter = 19,
        Sneak = 20,
        FakeSurrender = 21,
        Pickpocket = 22,
        ItemQuickThrow = 23,
        CoverSwap = 24,
        Hang = 25,
        Dismount = 26,
        Sidle = 27,
        ClimbDropDown = 28,
    };

    // Size: 0x4
    enum class EInterceptableGoalType : int32_t {
        CloseCombatFinisher = 0,
        MeleeAttack = 1,
        VaultFinisher = 2,
        AllAgilityActions = 3,
        GadgetActivation = 4,
        AllGadgetActivations = 5,
    };

    // Size: 0x1
    enum class EInterruptionConfiguration : int8_t {
        Default = 0,
        DoNotInterrupt = 1,
    };

    // Size: 0x1
    enum class EItemAttachmentState : int8_t {
        Attached = 0,
        Selected = 1,
        Equipped = 2,
    };

    // Size: 0x4
    enum class EItemOperationType : int32_t {
        None = 0,
        Attach = 1,
        Drop = 2,
        Max = 3,
    };

    // Size: 0x1
    enum class EKntCheckpointLoadState : int8_t {
        Initiate = 0,
        WaitForStreamingSets = 1,
        SpawnPlayer = 2,
        WaitForPlayerSpawner = 3,
        WaitForDynamicSpawners = 4,
        Finalize = 5,
    };

    // Size: 0x4
    enum class EKntMissionTimerType : int32_t {
        Timer = 0,
        Stopwatch = 1,
    };

    // Size: 0x1
    enum class ELocomotionSpeedOverrideType : int8_t {
        NoOverride = 0,
        Slow = 1,
        Medium = 2,
        Fast = 3,
    };

    // Size: 0x1
    enum class ELocomotionSpeedScalingModifier : int8_t {
        Avoidance = 0,
        Guidance = 1,
    };

    // Size: 0x4
    enum class ELookAtTargetMode : int32_t {
        InSocial = 0,
        Always = 1,
    };

    // Size: 0x2
    enum class ELureUnavailableFlag : int16_t {
        Disabled = 0,
        NotAvailable = 1,
        NotEnoughAgency = 2,
        InCooldown = 3,
        TargetInvalid = 4,
        TargetTooClose = 5,
        TargetInDifferentArea = 6,
        TargetPathTooFar = 7,
        PlayerBeingIdentifiedAsThreat = 8,
    };

    // Size: 0x4
    enum class EMeleeAlignmentBone : int32_t {
        Ground = 0,
        Impact = 1,
        Count = 2,
    };

    // Size: 0x4
    enum class EMeleeAlignmentType : int32_t {
        None = 0,
        AlignGroundBoneToOpponent = 1,
        AlignToTargetTransform = 2,
        AlignToVictim = 3,
        LinkToInstigatorBone = 4,
    };

    // Size: 0x4
    enum class EMeleeApproachType : int32_t {
        NotNeeded = 0,
        Run = 1,
        Vault = 2,
    };

    // Size: 0x4
    enum class EMeleeAttackAlignmentType : int32_t {
        GameplayDriven = 0,
        AnimationDriven_AlignToTargetBone = 1,
    };

    // Size: 0x1
    enum class EMeleeAttackOutcome : int8_t {
        None = 0,
        Successful = 1,
        Blocked = 2,
        Missed = 3,
    };

    // Size: 0x4
    enum class EMeleeAttackResult : int32_t {
        Unknown = 0,
        Missed_NoVictim = 1,
        Missed_VictimOutOfReach = 2,
        Missed_VictimSidestepped = 3,
        Missed_VictimCurrentlyNonAttackable = 4,
        Missed_VictimBlocked = 5,
        Missed_HitByMeleeAttackOnSameFrame = 6,
        Missed_NotAttackImpactReceived = 7,
        Missed_VictimDefensesBlock = 8,
        Missed_VictimDefensesMiss = 9,
        Successful_Unblocked = 11,
        Successful_BrokeBlock = 12,
        Count = 13,
    };

    // Size: 0x4
    enum class EMeleeAttackStep : int32_t {
        Inactive = 0,
        Preparing = 1,
        Preparing_WaitingForRetargeting = 2,
        Preparing_WaitingForSuccessCheck = 3,
        Attacking = 4,
        Impacting = 5,
        Missing = 6,
        BeingBlocked = 7,
        Recovering = 8,
    };

    // Size: 0x4
    enum class EMeleeAttackToleranceToHitPolicy : int32_t {
        ImpactBoneToTargetBone = 0,
        CapsuleToCapsule = 1,
        ImpactBoneToCapsule = 2,
    };

    // Size: 0x4
    enum class EMeleeAttackTriggerType : int32_t {
        QuickAttack = 0,
        ChargedAttack = 1,
    };

    // Size: 0x4
    enum class EMeleeAttackType : int32_t {
        Light = 0,
        Heavy = 1,
        LongLight = 2,
        LongHeavy = 3,
        Counter = 4,
        PistolAttack = 5,
        RifleAttack = 6,
        CombatRushShortRange = 7,
        CombatRushLongRange = 8,
        RedirectStrike = 9,
    };

    // Size: 0x4
    enum class EMeleeAttackTypeFlags : int32_t {
        Light = 1,
        Heavy = 2,
        LongLight = 4,
        LongHeavy = 8,
        Counter = 16,
        PistolAttack = 32,
        RifleAttack = 64,
        CombatRushShortRange = 128,
        CombatRushLongRange = 256,
        RedirectStrike = 512,
    };

    // Size: 0x4
    enum class EMeleeParryStep : int32_t {
        None = 0,
        Attacking = 1,
        Impacting = 2,
        Staggering = 3,
    };

    // Size: 0x4
    enum class EMeleeParryType : int32_t {
        None = 0,
        Parry = 1,
        Block = 2,
    };

    // Size: 0x4
    enum class EMeleeVariationType : int32_t {
        None = -1,
        Straight = 0,
        Left = 1,
        Right = 2,
    };

    // Size: 0x4
    enum class ENearDeathStateFinishReason : int32_t {
        Recovered = 0,
        Died = 1,
        Scripted = 2,
        Quit = 3,
    };

    // Size: 0x4
    enum class ENotificationType : int32_t {
        MainObjective = 0,
        SubObjective = 1,
        Hint = 2,
        QuestItem = 3,
        Notification = 4,
        Opportunity = 5,
        OpportunityStep = 6,
    };

    // Size: 0x4
    enum class ENpcArchetype : int32_t {
        Civilian = 0,
        Minion = 1,
        Henchman = 2,
        Brute = 3,
        BossAssassin = 4,
        PlayerCompanion = 5,
    };

    // Size: 0x4
    enum class ENpcReinforcementsState : int32_t {
        AvailableForRequest = 0,
        Requested = 1,
        ActiveFromSavegame = 2,
        Active = 3,
    };

    // Size: 0x4
    enum class ENpcStateIconType : int32_t {
        DistractedInAmbient = 0,
        EscalatingFromAmbient = 1,
        Manhunt = 2,
        ManhuntInvestigatingDisturbance = 3,
        Combat = 4,
        CombatDistracted = 5,
        CallingForReinforcements = 6,
        Confronting = 7,
        Bluffed = 8,
        Watcher = 9,
        Disoriented = 10,
        InfluencedByWatcher = 11,
        LtkWindowOfOpportunity = 12,
    };

    // Size: 0x4
    enum class EObjectiveDynamicDataCountDisplayType : int32_t {
        Default = 0,
        Currency = 1,
    };

    // Size: 0x4
    enum class EObjectiveDynamicDataType : int32_t {
        None = 0,
        Count_Up = 1,
        State_Progression = 2,
        Timer = 3,
        State_Progression_And_Percentage = 4,
    };

    // Size: 0x4
    enum class EObjectiveUIState : int32_t {
        Active = 0,
        Completed = 1,
        Deactivated = 2,
        Failed = 3,
    };

    // Size: 0x4
    enum class EObjectiveUIType : int32_t {
        MainObjective = 0,
        SubObjective = 1,
        Hint = 2,
        QuestItem = 3,
        Opportunity = 4,
        OpportunityStep = 5,
    };

    // Size: 0x1
    enum class EParryVariationType : int8_t {
        None = 0,
        Left = 1,
        Right = 2,
        BackLeft = 3,
        BackRight = 4,
    };

    // Size: 0x1
    enum class EPhysicsGameplayState : int8_t {
        PlayerStealth = 0,
        Vaulting = 1,
    };

    // Size: 0x1
    enum class EPhysicsObstacleStopType : int8_t {
        None = 0,
        Cliff = 1,
    };

    // Size: 0x1
    enum class EPickpocketMode : int8_t {
        Social = 0,
        Trespass = 1,
        Trespass_Bluffed = 2,
    };

    // Size: 0x4
    enum class EPlayerApproachingGoalCompletionMode : int32_t {
        BasedOnDistanceToTarget = 0,
        BasedOnDistanceAndFacingToTarget = 1,
    };

    // Size: 0x4
    enum class EPlayerApproachingGoalOrientationMode : int32_t {
        SameAsTarget = 0,
        SameAsApproachingDirection = 1,
        SameAsPlayerFacingAtStart = 2,
    };

    // Size: 0x4
    enum class EPlayerApproachingGoalStopRadiusMode : int32_t {
        AnyPositionWithinRadius = 0,
        PositionExactlyOnRadius = 1,
    };

    // Size: 0x4
    enum class EPlayerApproachingMovementType : int32_t {
        None = 0,
        CinematicFunnelingApproach = 1,
        CoreLogicGoal = 2,
        CoverCorner = 3,
        DashToCover = 4,
        UseDoor = 5,
        VirtualCapsule = 6,
        AgilityAction = 7,
        CameraFocusReorientation = 8,
        Complying = 9,
        FakeSurrender = 10,
        ActiveBluff = 11,
    };

    // Size: 0x1
    enum class EPlayerComplyingState : int8_t {
        None = 0,
        WaitingInitialDialogues = 1,
        FadingIn = 2,
        Teleporting = 3,
        FadingOut = 4,
    };

    // Size: 0x4
    enum class EPlayerGadgetTargetType : int32_t {
        None = 0,
        Humanoid = 1,
        Item = 2,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventAgilityActionTriggerMode : int32_t {
        Start = 0,
        End = 1,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventAgilityActionType : int32_t {
        Vault = 0,
        Scale = 1,
        ContextualJump = 2,
        Land = 3,
        LandRoll = 4,
        Crouch = 5,
        StandUp = 6,
        Dash = 7,
        Vent = 8,
        Cover = 9,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventContextSwitchType : int32_t {
        Entering = 0,
        Exiting = 1,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventContextType : int32_t {
        Any = 0,
        Public = 1,
        PublicFormal = 2,
        PublicCasual = 3,
        Trespassing = 4,
        LicenseToKill = 5,
        SituationContained = 6,
        SoftTrespassing = 7,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventLocomotionTrackingAreaConstraint : int32_t {
        None = 0,
        Public = 1,
        Trespassing = 2,
        LicenseToKill = 3,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventLocomotionTrackingSpatialConstraint : int32_t {
        None = 0,
        Cover = 1,
        Vent = 2,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventLocomotionTrackingStanceType : int32_t {
        CrouchIdle = 0,
        CrouchMoving = 1,
        Idle = 2,
        Walking = 3,
        Running = 4,
        Sprinting = 5,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventLocomotionTrackingTriggerMode : int32_t {
        AllConditionsAreSatified = 0,
        AllConditionsAreSatifiedAndStateChanges = 1,
    };

    // Size: 0x4
    enum class EPlayerGameplayEventType : int32_t {
        PlayerGameplayContextSwitch = 0,
        PlayerAgilityAction = 1,
        PlayerLocomotionTracking = 2,
    };

    // Size: 0x4
    enum class EPlayerGameplayRecordTimeStateFlag : int32_t {
        StandingUp = 0,
        Crouching = 1,
        Idle = 2,
        Moving = 3,
        Walking = 4,
        Running = 5,
        Sprinting = 6,
        InCover = 7,
        InsideVent = 8,
        Aiming = 9,
        InCloseCombat = 10,
        InWatchView = 11,
        InPublic = 12,
        InTrespassing = 13,
        InLicenseToKill = 14,
        Count = 15,
    };

    // Size: 0x4
    enum class EPlayerGameplaySystem : int32_t {
        CoverSystem = 0,
        HitReaction_Stumble = 1,
        Skill_Dash = 2,
        Skill_StartMeleeAttack = 3,
        Skill_StartSprinting = 4,
        Skill_Uncrouch = 5,
        Skill_Crouch = 6,
        Skill_QuickThrow = 7,
        Skill_AimedThrow = 8,
        Skill_Aiming = 9,
        Skill_PickupItem = 10,
        Skill_PickupFirearm = 11,
        Skill_SilentTakedown = 12,
        Skill_GadgetView = 13,
        Skill_GadgetActivation = 14,
        Skill_TrespassingStanceChange = 15,
        Skill_Pickpocket = 16,
        Skill_Whistle = 17,
        Skill_Interact = 18,
        Skill_ActiveBluff = 19,
        Skill_StopSprinting = 20,
        Skill_AttackFromAbove = 21,
        Skill_AgencyFocus = 22,
        AgencySystem = 23,
        AgilitySystem = 24,
        SituationContained = 25,
        CombatRush = 26,
        TouchTheWorldSystem = 27,
        VaultFinisher = 28,
        Skill_ContextualThrow = 29,
        Skill_ConfrontationOptIn = 30,
        Skill_Block = 31,
        Skill_Parry = 32,
        Skill_Grab = 33,
        Skill_CameraFocus = 34,
        Skill_Finisher = 35,
        Skill_GrabRush = 36,
        Skill_FakeSurrender = 37,
        Skill_Sidestep = 38,
        Skill_CombatRushPairedAction = 39,
        ConsumeGadgetResources = 40,
        GadgetViewSlowmo = 41,
        VaultDisarm = 42,
        SelectNextFirearm = 43,
        Skill_GrabQuickThrow = 44,
        Skill_Disarm = 45,
        Skill_CombatRushGrab = 46,
        Skill_DoorKick = 47,
        Skill_DoorRush = 48,
        TakedownAmmoReward = 49,
    };

    // Size: 0x1
    enum class EPlayerGameplaySystemDisabledStateLayerType : int8_t {
        Base = 0,
        Secondary = 1,
    };

    // Size: 0x4
    enum class EPlayerInputCollisionProbesModType : int32_t {
        DisableProbes = 0,
        ModifyMoveAlongSurfaceAngle = 1,
        ModifyProbesDistance = 2,
    };

    // Size: 0x4
    enum class EPlayerInputSchemeType : int32_t {
        Humanoid = 0,
        Vehicle = 1,
        Custom = 2,
        GameUI = 3,
        EngineUI = 4,
    };

    // Size: 0x1
    enum class EPlayerMovementDisabledSource : int8_t {
        CoreLogic = 0,
        FakeSurrender = 1,
        QuickFire = 2,
        AIDecisionMaking = 3,
        Confrontation = 4,
        Comply = 5,
    };

    // Size: 0x4
    enum class EPlayerMovementInputConstraintApplicationType : int32_t {
        Override = 0,
        Aggregate = 1,
    };

    // Size: 0x4
    enum class EPlayerMovementInputConstraintLayerType : int32_t {
        Base = 0,
        Secondary = 1,
    };

    // Size: 0x4
    enum class EPlayerMovementInputConstraintType : int32_t {
        InputSpeedLevel = 1,
        InputDeviceMagnitude = 2,
        InputDeviceDirection = 3,
        InputDeviceSmoothing = 4,
    };

    // Size: 0x1
    enum class EPlayerTargetLineOfSightReachType : int8_t {
        FirstContactWithTargetSurface = 0,
        TargetCenter = 1,
    };

    // Size: 0x1
    enum class EPlayerTargetLineOfSightType : int8_t {
        None = 0,
        SingleRay = 1,
        Sweep = 2,
        MultipleRayPoints = 3,
    };

    // Size: 0x1
    enum class EPlayerTargetSortMode : int8_t {
        DistanceToScreenCenter = 0,
        DistanceToCamera = 1,
        ScoreCurves = 2,
    };

    // Size: 0x1
    enum class EPlayerTargetState : int8_t {
        Locked = 0,
        Sticky = 1,
    };

    // Size: 0x4
    enum class EPlayerTargetStickinessType : int32_t {
        None = 0,
        DurationMaxScore = 1,
        DurationMinScore = 2,
        IncreasedAngle = 3,
    };

    // Size: 0x1
    enum class EPlayerTimeSlowDownSource : int8_t {
        AgencyFocus = 0,
        GadgetView = 1,
        LastKill = 2,
        CoreLogic = 3,
    };

    // Size: 0x4
    enum class EPlayerVirtualCapsuleObstacleType : int32_t {
        Invalid = 0,
        HighObstacle = 1,
        LowObstacle = 2,
    };

    // Size: 0x4
    enum class EPlayerVirtualCapsuleResultType : int32_t {
        None = 0,
        StopOnCollisionSurface = 1,
        StopOnCollisionWithNPC = 2,
        ApproachToStopOnCollisionSurface = 3,
        MovingTowardsCover = 4,
        RemapInputAlongCollisionSurface = 5,
        RemapInputForNPCCollisionAvoidance = 6,
    };

    // Size: 0x4
    enum class EPlayerVirtualCapsuleSufaceWidthEstimationType : int32_t {
        Invalid = 0,
        NoCollision = 1,
        DistanceToCorner = 2,
        SingleHitVirtualCapsule = 3,
        DistanceToSimulationEnd = 4,
    };

    // Size: 0x1
    enum class EPotentialHazardNpcReactionType : int8_t {
        Attract = 0,
        Repel = 1,
    };

    // Size: 0x1
    enum class EPrivateSpaceInvadedReactionRole : int8_t {
        None = 0,
        Invading = 1,
        Reacting = 2,
    };

    // Size: 0x1
    enum class EPrivateSpaceInvadedReactionType : int8_t {
        None = 0,
        Passive = 1,
        Mine = 2,
        InPlace = 3,
    };

    // Size: 0x1
    enum class EProjectileImpactBodyPart : int8_t {
        Torso = 0,
        Head = 1,
        LeftHand = 2,
        LeftArm = 3,
        LeftLeg = 4,
        RightHand = 5,
        RightArm = 6,
        RightLeg = 7,
        None = 8,
        Weapon = 9,
    };

    // Size: 0x1
    enum class EProjectileImpactOrigin : int8_t {
        FRONT = 0,
        BACK = 1,
        LEFT = 2,
        RIGHT = 3,
    };

    // Size: 0x4
    enum class ERangedCombatSituation : int32_t {
        Default = 0,
        TargetInCover = 1,
        TargetAimingFromCover = 2,
        TargetBackInSight = 3,
        TargetSprinting = 4,
        TargetDashing = 5,
        TargetVaulting = 6,
        TargetGettingShot = 7,
        TargetDoingFinisher = 8,
        TargetCombatRushing = 9,
        BeingShot = 10,
        Flanking = 11,
        DoingFocusedShot = 12,
        AfterFocusedShot = 13,
        BossAssassinBasement = 14,
        BossAssassinTakedownGracePeriod = 15,
        BossAssassinSniper = 16,
        DuringCloseCombat = 17,
        AfterCloseCombat = 18,
        HealthThresholdCap = 19,
        HitCap = 20,
        HitCap2 = 21,
        AfterFriendlyFire = 22,
        BossExoDamien = 23,
        TargetCoverDestroyed = 24,
    };

    // Size: 0x4
    enum class ERulesOfEngagementLethalForceAvailability : int32_t {
        Default = 0,
        Available = 1,
        Unavailable = 2,
    };

    // Size: 0x1
    enum class ESequenceControllerGpwEntityType : int8_t {
        Humanoid = 0,
        Vehicle = 1,
    };

    // Size: 0x1
    enum class ESequenceControllerGpwVehicleEntityType : int8_t {
        Car = 0,
        Boat = 1,
    };

    // Size: 0x1
    enum class ESicknessDartReaction : int8_t {
        Immune = 0,
        ShrugOff = 1,
        FullReaction = 2,
    };

    // Size: 0x1
    enum class ESideStepType : int8_t {
        Hop = 0,
        SideStep = 1,
    };

    // Size: 0x4
    enum class ESideStepWindowType : int32_t {
        Grab = 0,
        MeleeAttack = 1,
        TaserShot = 2,
    };

    // Size: 0x1
    enum class ESimulatedPlayerActiveSkill : int8_t {
        SimulatedSkill_Confrontation = 0,
        SimulatedSkill_FakeSurrender = 1,
    };

    // Size: 0x4
    enum class EStaggerSource : int32_t {
        ThrowableImpact = 0,
        MeleeImpact = 1,
        VaultImpact = 2,
        StaggeredMovementImpact = 3,
        GrabThrow = 4,
        Taser = 5,
        Flashbang = 6,
        CoreLogic = 7,
        Parry = 8,
        DoorImpact = 9,
        Firearm = 10,
        PrivateSpacePush = 11,
        Shockwave = 12,
        Explosion = 13,
        PairedAction = 14,
        AgilityActionImpact = 15,
        StumbleImpact = 16,
        CapsuleOverlap = 17,
        FlashMine = 18,
        ElectricalSurge = 19,
    };

    // Size: 0x4
    enum class EStaggerStrength : int32_t {
        Light = 0,
        Medium = 1,
        Heavy = 2,
    };

    // Size: 0x4
    enum class EStaggerStrengthFlags : int32_t {
        Light = 1,
        Medium = 2,
        Heavy = 4,
    };

    // Size: 0x1
    enum class EStaggeredLocomotionState : int8_t {
        MeleeImpact = 0,
        Generic = 1,
        FirearmImpact = 2,
        Idle = 3,
        WallImpact = 4,
        LowCoverImpact = 5,
        Death = 6,
        GrabThrow = 7,
        Parry = 8,
        WallDeath = 9,
        VaultAttackImpact = 10,
        LowCoverRoll = 11,
        HumanoidImpact = 12,
        ThrowableImpact = 13,
        DeathOverEdge = 14,
        PrivateSpacePush = 15,
        Explosion = 16,
        PairedAction = 17,
        Shockwave = 18,
        FlashMine = 19,
    };

    // Size: 0x4
    enum class EStanceChangeSource : int32_t {
        Undefined = 0,
        CrouchAimingInLowCover = 1,
        CrouchFromHighToLowCover = 2,
        CrouchGoal = 3,
        CrouchAfterScaling = 4,
        CrouchAfterLowCoverTakedown = 5,
        CrouchDashToLowCover = 6,
        UncrouchGoal = 7,
        UncrouchDashToHighCover = 8,
        UncrouchAimingInLowCover = 9,
        UncrouchOnCloseCombatHitReaction = 10,
        UncrouchWhenStaggered = 11,
        UncrouchWhenCloseCombat = 12,
        UncrouchWhenInConfrontation = 13,
        UncrouchWhenInSocialGameplayArea = 14,
        UncrouchWhenAccelerating = 15,
        MovingAndAiming = 16,
        Moving = 17,
        Aiming = 18,
        Idle = 19,
        AgilityAction = 20,
        CrouchAfterAttackFromAbove = 21,
    };

    // Size: 0x4
    enum class EStartMeleeAttackSkillAvailability : int32_t {
        Available = 0,
        Unavailable_AttackNotRequested = 1,
        Unavailable_GameplaySystemDisabled = 2,
        Unavailable_SoftTrespassing = 3,
        Unavailable_CombatDisabled = 4,
        Unavailable_MotorConfig = 5,
        Unavailable_GadgetView = 6,
        Unavailable_InteractionAnimation = 7,
        Unavailable_GrabCooldown = 8,
        Unavailable_ConflictingHumanoidData = 9,
        Unavailable_MeleeAttackingWithChainWindowClosed = 10,
        Unavailable_Grabbing = 11,
        Unavailable_NoValidTarget = 12,
        Unavailable_NoValidAttack = 13,
        Unavailable_AimTargetDriven = 14,
        Unavailable_NoValidTargetAfterCloseCombatKillCooldown = 15,
        Unavailable_OnGuide = 16,
        Count = 17,
    };

    // Size: 0x4
    enum class EStartMeleeAttackSkillVictimStatus : int32_t {
        Unknown = 0,
        Invalid_TooFarWhenDoingFakeSurrender = 1,
        Invalid_NoCandidateWithinMeleeAttackRange = 2,
        Valid = 3,
        Valid_FakeSurrenderTarget = 4,
        Valid_BestCandidateWithinMeleeAttackRange = 5,
    };

    // Size: 0x4
    enum class EStumbleCandidateType : int32_t {
        Undefined = 0,
        Humanoid = 1,
        Lowcover = 2,
        Railing = 3,
        Wall = 4,
        LowcoverRoll = 5,
        Entity = 6,
        Count = 7,
    };

    // Size: 0x4
    enum class EStumbleDamageEventType : int32_t {
        OnStumble = 0,
        OnAnimationEvent = 1,
    };

    // Size: 0x2
    enum class EStumbleImpactFlag : int16_t {
        BlockingImpact = 0,
        Heavy = 1,
        CausesRollOver = 2,
        AllowsContextualAttack = 3,
        StunsInstigator = 4,
        UnequipWeapon = 5,
        FallbackStumble = 6,
        IsDamageableObject = 7,
        IsDoor = 8,
        WillDestroyObject = 9,
        StaggerStumbledTarget = 10,
        LandedDuringRollOver = 11,
        OverrideStumble = 12,
    };

    // Size: 0x4
    enum class EStumbleImpactType : int32_t {
        Unknown = 0,
        HighWall = 1,
        LowWall = 2,
        Humanoid = 3,
        RailingDeath = 4,
        LowCoverRoll = 5,
    };

    // Size: 0x4
    enum class EStumbleListenerImpactType : int32_t {
        Stumble = 0,
        ContextualPairedAction = 1,
    };

    // Size: 0x1
    enum class EStunType : int8_t {
        Taser = 0,
        Flashbang = 1,
        SlipAndKnockout = 2,
        Stagger = 3,
        Parry = 4,
        Invalid = -1,
    };

    // Size: 0x4
    enum class ETWILeaveContext : int32_t {
        None = 0,
        StopInProgress = 1,
        Leaving = 2,
    };

    // Size: 0x4
    enum class ETWIRuntimeState : int32_t {
        Idle = 0,
        Searching = 1,
        CloseButNoSurfaceHit = 2,
        SurfaceHit = 3,
        Aborted = 4,
        OnCooldown = 5,
    };

    // Size: 0x4
    enum class ETWIStopContext : int32_t {
        None = 0,
        HighObstacleStop = 1,
        HighObstaclePredictedStop = 2,
        LowObstacleStop = 3,
        LowObstaclePredictedStop = 4,
    };

    // Size: 0x1
    enum class ETargetingRaycastDetailLevel : int8_t {
        Simple = 0,
        Bones = 1,
        Detailed = 2,
    };

    // Size: 0x1
    enum class ETraversalRaycastHitAspectFlag : int8_t {
        SlidingSurface = 0,
    };

    // Size: 0x1
    enum class ETraversalSearchResultType : int8_t {
        None = 0,
        Edge = 1,
        Ramp = 2,
        BlockingObstacle = 3,
        NarrowObstacle = 4,
        ObstacleToClimb = 5,
        ObstacleToVault = 6,
        ObstacleToJump = 7,
        ObstacleVaultDisabled = 8,
        ObstacleClimbDisabled = 9,
        ObstacleMarkupWithAgilityDisabled = 10,
        DisallowedApproachAngle = 11,
    };

    // Size: 0x1
    enum class ETurnCoverCornerType : int8_t {
        NotAvailable = 0,
        AvailableInactive = 1,
        AvailableReady = 2,
    };

    // Size: 0x1
    enum class EUseDoorSkillType : int8_t {
        Open = 0,
        Close = 1,
        Kick = 2,
        Bullrush = 3,
        Sabotage = 4,
        TryUseSabotagedDoor = 5,
    };

    // Size: 0x4
    enum class EVaultObstacleDepthVariations : int32_t {
        Fence = 0,
        ShortestLowObstacle = 1,
        ShortLowObstacle = 2,
        DeepLowObstacle = 3,
        DeepestLowObstacle = 4,
    };

    // Size: 0x1
    enum class EVaultType : int8_t {
        VaultDefault = 0,
        VaultAttack = 1,
        VaultFinisher = 2,
        VaultDisarm = 3,
        None = 4,
    };

    // Size: 0x1
    enum class EVehicleAerodynamicsDownforceMode : int8_t {
        Always = 0,
        OnlyOnGround = 1,
        OnlyOnAir = 2,
    };

    // Size: 0x4
    enum class EVehicleApplyForceSource : int32_t {
        Aerodynamics = 0,
        ExternalSource = 1,
        CollisionAssist = 2,
    };

    // Size: 0x4
    enum class EVehicleAttachmentsDriveMode : int32_t {
        PhysicsDriven = 0,
        AnimationDriven = 1,
        MixOfPhysicsAndAnimation = 2,
    };

    // Size: 0x4
    enum class EVehicleBoatAnimationState : int32_t {
        NotInBoat = 0,
        Entering = 1,
        Driving = 2,
        Passanger = 3,
        Exiting = 4,
    };

    // Size: 0x4
    enum class EVehicleBoatCollisionObjectType : int32_t {
        Wall = 0,
        AnotherBoat = 1,
    };

    // Size: 0x4
    enum class EVehicleBoatHardCollisionAssistType : int32_t {
        NotColliding = 0,
        AutoSteeringWithSideCollision = 1,
        UsingPhysicsCollision = 2,
        ArtificialBounceOffSurface = 3,
        HardStop = 4,
    };

    // Size: 0x4
    enum class EVehicleBoatHumanoidAttachmentParent : int32_t {
        PhysicsSpatial = 0,
        VisualSpatial = 1,
    };

    // Size: 0x4
    enum class EVehicleBoatNPCHangPointType : int32_t {
        None = -1,
        N_0 = 0,
        NE_1 = 1,
        SE_2 = 2,
        SW_3 = 3,
        NW_4 = 4,
    };

    // Size: 0x1
    enum class EVehicleBoatVisualsMode : int8_t {
        SimpleMesh = 0,
        AnimatedMesh = 1,
    };

    // Size: 0x4
    enum class EVehicleCameraStateType : int32_t {
        AtRest = 0,
        Rolling = 1,
        Accelerating = 2,
        Braking = 3,
    };

    // Size: 0x4
    enum class EVehicleCameraTargetReinitializationType : int32_t {
        None = 0,
        InitializeWithEnteringVehicleCamera = 1,
        InitializeWithDrivingCamera = 2,
    };

    // Size: 0x4
    enum class EVehicleCameraTargetSpringFollowMode : int32_t {
        SpeedBasedResponsiveness = 0,
        AngleBasedResponsiveness = 1,
        SpeedBasedSimpleVehicleSpring = 2,
    };

    // Size: 0x4
    enum class EVehicleCameraTargetSpringFollowState : int32_t {
        InitializeWithEnteringVehicleCamera = 0,
        InitializeWithDrivingCamera = 1,
        InputBasedSprings = 2,
        CooldownSprings = 3,
        InputAssistSpringFollow = 4,
        InputTriggeredReset = 5,
        VehiclePhysicsSprings = 6,
    };

    // Size: 0x4
    enum class EVehicleCameraTargetType : int32_t {
        Default = 0,
        VehiclePhysicsSpringFollowTarget = 1,
        VehiclePhysicsSpringLookAtTarget = 2,
    };

    // Size: 0x4
    enum class EVehicleCameraType : int32_t {
        BasedOnInputState = 0,
        BasedOnVehiclePhysics = 1,
    };

    // Size: 0x1
    enum class EVehicleCarAnimationState : int8_t {
        None = 0,
        Entering = 1,
        Driving = 2,
        Exiting = 3,
    };

    // Size: 0x1
    enum class EVehicleCarComponentType : int8_t {
        Drifting = 0,
        CollisionHandling = 1,
        Aerodynamics = 2,
        CameraController = 3,
    };

    // Size: 0x4
    enum class EVehicleCarConfigOverrideType : int32_t {
        Acceleration = 0,
        RevAcceleration = 1,
        Deceleration = 2,
        Steering = 3,
        Tires = 4,
        WheelsSteeringAndBrakes = 5,
        WheelsAntiroll = 6,
    };

    // Size: 0x4
    enum class EVehicleCinematicSpeedMode : int32_t {
        None = 0,
        ForwardSpeed = 1,
        RigidbodyVelocity = 2,
    };

    // Size: 0x1
    enum class EVehicleCollisionCorrectionMode : int8_t {
        ImpactForceImpulse = 0,
        ConstantLateralForce = 1,
    };

    // Size: 0x4
    enum class EVehicleDebugLogCategory : int32_t {
        SimulationPauseResume = 0,
        Aerodynamics = 1,
        Teleport = 2,
        ExternalForces = 3,
        SpeedModifiers = 4,
        CollisionDetection = 5,
        DataChanges = 6,
        Count = 7,
    };

    // Size: 0x4
    enum class EVehicleDriftingConfigType : int32_t {
        Brake = 0,
        Handbrake = 1,
        Autodrift = 2,
    };

    // Size: 0x4
    enum class EVehicleForceType : int32_t {
        Impulse = 0,
        PointImpulse = 1,
        Force = 2,
        PointForce = 3,
        TorqueImpulse = 4,
        Torque = 5,
    };

    // Size: 0x1
    enum class EVehicleHumanoidAttachMode : int8_t {
        Instant = 0,
        AnimationDriven = 1,
    };

    // Size: 0x1
    enum class EVehicleHumanoidAttachmentParent : int8_t {
        PhysicsChassis = 0,
        SkeletonSpatial = 1,
        VehicleSpatialAttachment = 2,
    };

    // Size: 0x1
    enum class EVehicleHumanoidVisibility : int8_t {
        Visible = 0,
        Invisible = 1,
    };

    // Size: 0x4
    enum class EVehiclePhysicalPartType : int32_t {
        WheelFrontLeft = 0,
        WheelFrontRight = 1,
        WheelRearLeft = 2,
        WheelRearRight = 3,
        Chassis = 4,
    };

    // Size: 0x4
    enum class EVehicleSetAngularVelocitySource : int32_t {
        AerodynamicPitchConstraint = 0,
        AerodynamicRollConstraint = 1,
    };

    // Size: 0x4
    enum class EVehicleSkeletonSpatialSynchronizationMode : int32_t {
        Disabled = 0,
        PhysicsTransform = 1,
    };

    // Size: 0x4
    enum class EVehicleSlopeState : int32_t {
        Invalid = 0,
        Airbone = 1,
        Uphill = 2,
        Downhill = 3,
        Flat = 4,
    };

    // Size: 0x4
    enum class EVehicleSpatialAttachmentLocalOffsetType : int32_t {
        None = 0,
        Inline = 1,
        External = 2,
    };

    // Size: 0x4
    enum class EVehicleSpatialAttachmentMode : int32_t {
        WorldPositionAndRotation = 0,
        WorldPosition = 1,
        WorldRotation = 2,
        WorldPositionPhysicsBodyRotation = 3,
        WorldPositionSpeedBasedWheelRotation = 4,
        SkeletonBoneTransformFromPhysicalPart = 5,
        SkeletonBoneTransformCustom = 6,
        CustomVehicleSpatialPositionSpeedBasedWheelRotation = 7,
    };

    // Size: 0x4
    enum class EVehicleSpatialAttachmentOscilationMethod : int32_t {
        Sin = 0,
        Cos = 1,
    };

    // Size: 0x4
    enum class EVehicleSpatialAttachmentOscilationType : int32_t {
        Yaw = 0,
        Pitch = 1,
        Roll = 2,
    };

    // Size: 0x4
    enum class EVehicleVelocityModifier : int32_t {
        SetForwardSpeedSingleFrame = 0,
        AddMaxSpeedLimit = 1,
        RemoveMaxSpeedLimit = 2,
    };

    // Size: 0x4
    enum class EVehicleWheelIndex : int32_t {
        FrontLeft = 0,
        FrontRight = 1,
        RearLeft = 2,
        RearRight = 3,
    };

    // Size: 0x4
    enum class EVirtualCapsuleApproachReasonType : int32_t {
        None = 0,
        EarlyExit = 1,
        InCover = 2,
        AnticipationWithApproach = 3,
        CollisionStopWithoutApproach = 4,
        InputRemap = 5,
        NoVirtualCapsuleResult = 6,
    };

    // Size: 0x4
    enum class EVirtualCapsuleApproachResultType : int32_t {
        None = 0,
        AddApproach = 1,
        RemoveApproachImmediately = 2,
        RemoveApproachWhenFinished = 3,
    };

    // Size: 0x4
    enum class EVirtualCapsuleNPCCollisionType : int32_t {
        Invalid = -1,
        Front = 0,
        LateralLeft = 1,
        LateralRight = 2,
    };

    // Size: 0x4
    enum class EVisualAggressionLevel : int32_t {
        None = 0,
        Soft = 1,
        Hard = 2,
    };

    // Size: 0x4
    enum class PhysicalGoalStatus : int32_t {
        CreatedByPrimitiveTask = 0,
        ScheduledToBeAbortedFromPrimitiveTask = 1,
        PassedSolver = 2,
        RejectedBySolver = 3,
        AbortedBySolver = 4,
        AbortedNoSkill = 5,
        AbortedByAutomatedRemoval = 6,
        AbortedFromPrimitiveTask = 7,
        InSkill = 8,
        Failed = 9,
        Succeeded = 10,
    };

    namespace PlayerInteractions {
        // Size: 0x4
        enum class EInteractionAnimationOperationResult : int32_t {
            None = 0,
            StartRequestAccepted = 1,
            StartRequestRejected = 2,
            InteractionCompleted = 3,
            InteractionInterrupted = 4,
        };

        // Size: 0x4
        enum class EType : int32_t {
            Standard = 0,
            Pickpocket = 1,
        };

    }
}

#include "ConsoleObserver.h"
#include "DataThread.h"
#include "../gui/NaMainWindow.h"
#include "../entity_model/Entity.h"
#include "../entity_model/Ontology.h"
#include "../entity_model/AnnotationSession.h"
#include "../utility/JacsUtil.h"
#include "../../webservice/impl/EntityAdapter.h"
#include "../../webservice/console/cdsConsoleDataServiceProxy.h"

ConsoleObserver::ConsoleObserver(NaMainWindow *naMainWindow, QObject *parent) :
    QObject(parent),
    mainWindow(naMainWindow),
    loadOntologyThread(0),
    entityViewRequestedThread(0),
    annotatedBranchViewRequestedThread(0),
    annotationsChangedThread(0),
    loadAnnotationSessionThread(0),
    loadOntologyMutex(QMutex::Recursive),
    entityViewRequestedMutex(QMutex::Recursive),
    annotatedBranchViewRequestedMutex(QMutex::Recursive),
    annotationsChangedMutex(QMutex::Recursive),
    loadAnnotationSessionMutex(QMutex::Recursive)
{
}

//*******************************************************************************************
// ontologySelected event
//*******************************************************************************************

void ConsoleObserver::loadOntology(qint64 rootId)
{
    QMutexLocker locker(&loadOntologyMutex);

    if (loadOntologyThread != NULL)
    {
        loadOntologyThread->disregard();
    }

    loadOntologyThread = new GetOntologyThread(rootId);
    connect(loadOntologyThread, SIGNAL(gotResults(const void *)),
            this, SLOT(loadOntologyResults(const void *)));
    connect(loadOntologyThread, SIGNAL(gotError(const QString &)),
            this, SLOT(loadOntologyError(const QString &)));
    loadOntologyThread->start(QThread::NormalPriority);
}

void ConsoleObserver::loadOntologyResults(const void *results)
{
    QMutexLocker locker(&loadOntologyMutex);

    emit openOntology((Ontology *)results);
    loadOntologyThread = NULL;
}

void ConsoleObserver::loadOntologyError(const QString &error)
{
    QMutexLocker locker(&loadOntologyMutex);

    emit openOntology(0);
    emit communicationError(error);
    loadOntologyThread = NULL;
}

void ConsoleObserver::ontologySelected(qint64 rootId)
{
    QMutexLocker locker(&loadOntologyMutex);

    qDebug() << "Got signal ontologySelected:" << rootId;
    loadOntology(rootId);
}


//*******************************************************************************************
// ontologyChanged event
//*******************************************************************************************

void ConsoleObserver::ontologyChanged(qint64 rootId)
{
    qDebug() << "Got signal ontologyChanged:" << rootId;
    loadOntology(rootId);
}

//*******************************************************************************************
// entitySelected event
//*******************************************************************************************

void ConsoleObserver::entitySelected(qint64 entityId, bool outline)
{
    qDebug() << "Got signal entitySelected:" << entityId << "isOutline?=" << outline;
    emit selectEntityById(entityId, true);
}

//*******************************************************************************************
// entityViewRequested event
//*******************************************************************************************

void ConsoleObserver::entityViewRequested(qint64 entityId)
{
    QMutexLocker locker(&entityViewRequestedMutex);

    qDebug() << "Got signal entityViewRequested:" << entityId;

    if (entityViewRequestedThread != NULL)
    {
        entityViewRequestedThread->disregard();
    }

    entityViewRequestedThread = new GetEntityThread(entityId);
    connect(entityViewRequestedThread, SIGNAL(gotResults(const void *)),
            this, SLOT(entityViewRequestedResults(const void *)));
    connect(entityViewRequestedThread, SIGNAL(gotError(const QString &)),
            this, SLOT(entityViewRequestedError(const QString &)));
    entityViewRequestedThread->start(QThread::NormalPriority);
}

void ConsoleObserver::entityViewRequestedResults(const void *results)
{
    QMutexLocker locker(&entityViewRequestedMutex);

    Entity *entity = (Entity *)results;
    if (entity == NULL) return;

    QString type = *entity->entityType;
    if (type == "Neuron Separator Pipeline Result")
    {
        QApplication::alert((QWidget *)mainWindow, 10000);
        mainWindow->show();
        mainWindow->activateWindow();
        mainWindow->raise();
        // TODO: begin process indication
        annotatedBranchViewRequested(*entity->id);
    }

    entityViewRequestedThread = NULL;
}

void ConsoleObserver::entityViewRequestedError(const QString & error)
{
    QMutexLocker locker(&entityViewRequestedMutex);

    emit communicationError(error);
    entityViewRequestedThread = NULL;
}

//*******************************************************************************************
// annotatedBranchViewRequested event
//*******************************************************************************************

void ConsoleObserver::annotatedBranchViewRequested(qint64 entityId)
{
    QMutexLocker locker(&annotatedBranchViewRequestedMutex);

    qDebug() << "Got signal annotatedBranchViewRequested:" << entityId;

    if (annotatedBranchViewRequestedThread != NULL)
    {
        annotatedBranchViewRequestedThread->disregard();
    }

    annotatedBranchViewRequestedThread = new GetAnnotatedBranchThread(entityId);
    connect(annotatedBranchViewRequestedThread, SIGNAL(gotResults(const void *)),
            this, SLOT(annotatedBranchViewRequestedResults(const void *)));
    connect(annotatedBranchViewRequestedThread, SIGNAL(gotError(const QString &)),
            this, SLOT(annotatedBranchViewRequestedError(const QString &)));
    annotatedBranchViewRequestedThread->start(QThread::NormalPriority);
}

void ConsoleObserver::annotatedBranchViewRequestedResults(const void *results)
{
    QMutexLocker locker(&annotatedBranchViewRequestedMutex);

    AnnotatedBranch *annotatedBranch = (AnnotatedBranch *)results;
    if (annotatedBranch == NULL) return;

    QString filepath = annotatedBranch->getFilePath();
    qDebug() << "Opening annotated branch, Name:"<<annotatedBranch->name()<<"FilePath:"<<filepath;

    if (!filepath.isEmpty()) {
        emit openAnnotatedBranch(annotatedBranch);
    }

    annotatedBranchViewRequestedThread = NULL;
}

void ConsoleObserver::annotatedBranchViewRequestedError(const QString & error)
{
    QMutexLocker locker(&annotatedBranchViewRequestedMutex);

    emit communicationError(error);
    annotatedBranchViewRequestedThread = NULL;
}

//*******************************************************************************************
// annotationsChanged event
//*******************************************************************************************

void ConsoleObserver::annotationsChanged(qint64 entityId)
{
    QMutexLocker locker(&annotationsChangedMutex);

    qDebug() << "Got signal annotationsChanged:" << entityId;

    if (annotationsChangedThread != NULL)
    {
        annotationsChangedThread->disregard();
    }

    annotationsChangedThread = new GetEntityAnnotationsThread(entityId);
    connect(annotationsChangedThread, SIGNAL(gotResults(const void *)),
            this, SLOT(annotationsChangedResults(const void *)));
    connect(annotationsChangedThread, SIGNAL(gotError(const QString &)),
            this, SLOT(annotationsChangedError(const QString &)));
    annotationsChangedThread->start(QThread::NormalPriority);
}

void ConsoleObserver::annotationsChangedResults(const void *results)
{
    QMutexLocker locker(&annotationsChangedMutex);
    GetEntityAnnotationsThread *t = (GetEntityAnnotationsThread *)annotationsChangedThread;
    emit updateAnnotations(t->getEntityId(), (AnnotationList *)results, t->getUserColorMap());
    annotationsChangedThread = NULL;
}

void ConsoleObserver::annotationsChangedError(const QString & error)
{
    QMutexLocker locker(&annotationsChangedMutex);
    emit communicationError(error);
    annotationsChangedThread = NULL;
}

//*******************************************************************************************
// sessionSelected event
//*******************************************************************************************

void ConsoleObserver::sessionSelected(qint64 sessionId)
{
    QMutexLocker locker(&loadAnnotationSessionMutex);

    qDebug() << "Got signal sessionSelected:" << sessionId;

    if (loadAnnotationSessionThread != NULL)
    {
        loadAnnotationSessionThread->disregard();
    }

    loadAnnotationSessionThread = new GetAnnotationSessionThread(sessionId);
    connect(loadAnnotationSessionThread, SIGNAL(gotResults(const void *)),
            this, SLOT(loadAnnotationSessionResults(const void *)));
    connect(loadAnnotationSessionThread, SIGNAL(gotError(const QString &)),
            this, SLOT(loadAnnotationSessionError(const QString &)));
    loadAnnotationSessionThread->start(QThread::NormalPriority);
}

void ConsoleObserver::loadAnnotationSessionResults(const void *results)
{
    QMutexLocker locker(&loadAnnotationSessionMutex);

    emit openAnnotationSession((AnnotationSession *)results);
    loadAnnotationSessionThread = NULL;
}

void ConsoleObserver::loadAnnotationSessionError(const QString & error)
{
    QMutexLocker locker(&loadAnnotationSessionMutex);

    emit communicationError(error);
    loadAnnotationSessionThread = NULL;
}


//*******************************************************************************************
// sessionDeselected event
//*******************************************************************************************

void ConsoleObserver::sessionDeselected()
{
    qDebug() << "Got signal sessionDeselected";

    emit closeAnnotationSession();
}

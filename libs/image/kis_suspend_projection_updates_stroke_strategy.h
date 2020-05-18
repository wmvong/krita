/*
 *  Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __KIS_SUSPEND_PROJECTION_UPDATES_STROKE_STRATEGY_H
#define __KIS_SUSPEND_PROJECTION_UPDATES_STROKE_STRATEGY_H

#include <KisRunnableBasedStrokeStrategy.h>

#include <QScopedPointer>

class KisSuspendProjectionUpdatesStrokeStrategy : public KisRunnableBasedStrokeStrategy
{
public:
    struct SharedData {
        KisProjectionUpdatesFilterCookie installedFilterCookie = {};
    };
    using SharedDataSP = QSharedPointer<SharedData>;

public:
    KisSuspendProjectionUpdatesStrokeStrategy(KisImageWSP image, bool suspend, SharedDataSP sharedData);
    ~KisSuspendProjectionUpdatesStrokeStrategy() override;

    static QList<KisStrokeJobData*> createSuspendJobsData(KisImageWSP image);
    static QList<KisStrokeJobData*> createResumeJobsData(KisImageWSP image);
    static SharedDataSP createSharedData();
private:
    void initStrokeCallback() override;
    void doStrokeCallback(KisStrokeJobData *data) override;
    void cancelStrokeCallback() override;

    void suspendStrokeCallback() override;
    void resumeStrokeCallback() override;


    void resumeAndIssueUpdates(bool dropUpdates);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif /* __KIS_SUSPEND_PROJECTION_UPDATES_STROKE_STRATEGY_H */

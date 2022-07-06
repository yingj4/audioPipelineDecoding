; ModuleID = 'src//kiss_fft.cpp'
source_filename = "src//kiss_fft.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.kiss_fft_state = type { i32, i32, [64 x i32], [1 x %struct.kiss_fft_cpx] }
%struct.kiss_fft_cpx = type { float, float }

; Function Attrs: nofree nounwind uwtable
define dso_local %struct.kiss_fft_state* @kiss_fft_alloc(i32 %nfft, i32 %inverse_fft, i8* %mem, i64* %lenmem) local_unnamed_addr #0 {
entry:
  %sub = add nsw i32 %nfft, -1
  %conv = sext i32 %sub to i64
  %mul = shl nsw i64 %conv, 3
  %add = add nsw i64 %mul, 272
  %cmp = icmp eq i64* %lenmem, null
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %call = tail call noalias i8* @malloc(i64 %add) #7
  %0 = bitcast i8* %call to %struct.kiss_fft_state*
  br label %if.end4

if.else:                                          ; preds = %entry
  %cmp1 = icmp eq i8* %mem, null
  br i1 %cmp1, label %if.end, label %land.lhs.true

land.lhs.true:                                    ; preds = %if.else
  %1 = load i64, i64* %lenmem, align 8, !tbaa !2
  %cmp2 = icmp ult i64 %1, %add
  %2 = bitcast i8* %mem to %struct.kiss_fft_state*
  %spec.select = select i1 %cmp2, %struct.kiss_fft_state* null, %struct.kiss_fft_state* %2
  br label %if.end

if.end:                                           ; preds = %land.lhs.true, %if.else
  %st.0 = phi %struct.kiss_fft_state* [ null, %if.else ], [ %spec.select, %land.lhs.true ]
  store i64 %add, i64* %lenmem, align 8, !tbaa !2
  br label %if.end4

if.end4:                                          ; preds = %if.end, %if.then
  %st.1 = phi %struct.kiss_fft_state* [ %0, %if.then ], [ %st.0, %if.end ]
  %tobool = icmp eq %struct.kiss_fft_state* %st.1, null
  br i1 %tobool, label %if.end24, label %if.then5

if.then5:                                         ; preds = %if.end4
  %nfft6 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 0
  store i32 %nfft, i32* %nfft6, align 4, !tbaa !6
  %inverse = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 1
  store i32 %inverse_fft, i32* %inverse, align 4, !tbaa !9
  %cmp753 = icmp sgt i32 %nfft, 0
  %conv10 = sitofp i32 %nfft to double
  br i1 %cmp753, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %if.then5
  %3 = fdiv fast double 1.000000e+00, %conv10
  %tobool12 = icmp eq i32 %inverse_fft, 0
  %wide.trip.count = zext i32 %nfft to i64
  %xtraiter = and i64 %wide.trip.count, 1
  %4 = icmp eq i32 %nfft, 1
  br i1 %tobool12, label %for.body.us.preheader, label %for.body.preheader

for.body.preheader:                               ; preds = %for.body.lr.ph
  br i1 %4, label %for.end.loopexit62.unr-lcssa, label %for.body.preheader.new

for.body.preheader.new:                           ; preds = %for.body.preheader
  %unroll_iter65 = sub nsw i64 %wide.trip.count, %xtraiter
  br label %for.body

for.body.us.preheader:                            ; preds = %for.body.lr.ph
  br i1 %4, label %for.end.loopexit.unr-lcssa, label %for.body.us.preheader.new

for.body.us.preheader.new:                        ; preds = %for.body.us.preheader
  %unroll_iter = sub nsw i64 %wide.trip.count, %xtraiter
  br label %for.body.us

for.body.us:                                      ; preds = %for.body.us, %for.body.us.preheader.new
  %indvars.iv = phi i64 [ 0, %for.body.us.preheader.new ], [ %indvars.iv.next.1, %for.body.us ]
  %niter = phi i64 [ %unroll_iter, %for.body.us.preheader.new ], [ %niter.nsub.1, %for.body.us ]
  %5 = trunc i64 %indvars.iv to i32
  %conv8.us = sitofp i32 %5 to double
  %mul9.us = fmul fast double %conv8.us, 0xC01921FB54442D18
  %6 = fmul fast double %mul9.us, %3
  %7 = tail call fast double @llvm.cos.f64(double %6)
  %conv16.us = fptrunc double %7 to float
  %r.us = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv, i32 0
  store float %conv16.us, float* %r.us, align 4, !tbaa !10
  %8 = tail call fast double @llvm.sin.f64(double %6)
  %conv17.us = fptrunc double %8 to float
  %i22.us = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv, i32 1
  store float %conv17.us, float* %i22.us, align 4, !tbaa !13
  %indvars.iv.next = or i64 %indvars.iv, 1
  %9 = trunc i64 %indvars.iv.next to i32
  %conv8.us.1 = sitofp i32 %9 to double
  %mul9.us.1 = fmul fast double %conv8.us.1, 0xC01921FB54442D18
  %10 = fmul fast double %mul9.us.1, %3
  %11 = tail call fast double @llvm.cos.f64(double %10)
  %conv16.us.1 = fptrunc double %11 to float
  %r.us.1 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv.next, i32 0
  store float %conv16.us.1, float* %r.us.1, align 4, !tbaa !10
  %12 = tail call fast double @llvm.sin.f64(double %10)
  %conv17.us.1 = fptrunc double %12 to float
  %i22.us.1 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv.next, i32 1
  store float %conv17.us.1, float* %i22.us.1, align 4, !tbaa !13
  %indvars.iv.next.1 = add nuw nsw i64 %indvars.iv, 2
  %niter.nsub.1 = add i64 %niter, -2
  %niter.ncmp.1 = icmp eq i64 %niter.nsub.1, 0
  br i1 %niter.ncmp.1, label %for.end.loopexit.unr-lcssa, label %for.body.us

for.body:                                         ; preds = %for.body, %for.body.preheader.new
  %indvars.iv56 = phi i64 [ 0, %for.body.preheader.new ], [ %indvars.iv.next57.1, %for.body ]
  %niter66 = phi i64 [ %unroll_iter65, %for.body.preheader.new ], [ %niter66.nsub.1, %for.body ]
  %13 = trunc i64 %indvars.iv56 to i32
  %conv8 = sitofp i32 %13 to double
  %mul9 = fmul fast double %conv8, 0xC01921FB54442D18
  %14 = fmul fast double %mul9, %3
  %15 = tail call fast double @llvm.cos.f64(double %14)
  %conv16 = fptrunc double %15 to float
  %r = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv56, i32 0
  store float %conv16, float* %r, align 4, !tbaa !10
  %16 = call fast double @llvm.sin.f64(double %14)
  %17 = fptrunc double %16 to float
  %conv17 = fsub fast float -0.000000e+00, %17
  %i22 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv56, i32 1
  store float %conv17, float* %i22, align 4, !tbaa !13
  %indvars.iv.next57 = or i64 %indvars.iv56, 1
  %18 = trunc i64 %indvars.iv.next57 to i32
  %conv8.1 = sitofp i32 %18 to double
  %mul9.1 = fmul fast double %conv8.1, 0xC01921FB54442D18
  %19 = fmul fast double %mul9.1, %3
  %20 = tail call fast double @llvm.cos.f64(double %19)
  %conv16.1 = fptrunc double %20 to float
  %r.1 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv.next57, i32 0
  store float %conv16.1, float* %r.1, align 4, !tbaa !10
  %21 = call fast double @llvm.sin.f64(double %19)
  %22 = fptrunc double %21 to float
  %conv17.1 = fsub fast float -0.000000e+00, %22
  %i22.1 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv.next57, i32 1
  store float %conv17.1, float* %i22.1, align 4, !tbaa !13
  %indvars.iv.next57.1 = add nuw nsw i64 %indvars.iv56, 2
  %niter66.nsub.1 = add i64 %niter66, -2
  %niter66.ncmp.1 = icmp eq i64 %niter66.nsub.1, 0
  br i1 %niter66.ncmp.1, label %for.end.loopexit62.unr-lcssa, label %for.body

for.end.loopexit.unr-lcssa:                       ; preds = %for.body.us, %for.body.us.preheader
  %indvars.iv.unr = phi i64 [ 0, %for.body.us.preheader ], [ %indvars.iv.next.1, %for.body.us ]
  %lcmp.mod = icmp eq i64 %xtraiter, 0
  br i1 %lcmp.mod, label %for.end, label %for.body.us.epil

for.body.us.epil:                                 ; preds = %for.end.loopexit.unr-lcssa
  %23 = trunc i64 %indvars.iv.unr to i32
  %conv8.us.epil = sitofp i32 %23 to double
  %mul9.us.epil = fmul fast double %conv8.us.epil, 0xC01921FB54442D18
  %24 = fmul fast double %mul9.us.epil, %3
  %25 = tail call fast double @llvm.cos.f64(double %24)
  %conv16.us.epil = fptrunc double %25 to float
  %r.us.epil = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv.unr, i32 0
  store float %conv16.us.epil, float* %r.us.epil, align 4, !tbaa !10
  %26 = tail call fast double @llvm.sin.f64(double %24)
  %conv17.us.epil = fptrunc double %26 to float
  %i22.us.epil = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv.unr, i32 1
  store float %conv17.us.epil, float* %i22.us.epil, align 4, !tbaa !13
  br label %for.end

for.end.loopexit62.unr-lcssa:                     ; preds = %for.body, %for.body.preheader
  %indvars.iv56.unr = phi i64 [ 0, %for.body.preheader ], [ %indvars.iv.next57.1, %for.body ]
  %lcmp.mod64 = icmp eq i64 %xtraiter, 0
  br i1 %lcmp.mod64, label %for.end, label %for.body.epil

for.body.epil:                                    ; preds = %for.end.loopexit62.unr-lcssa
  %27 = trunc i64 %indvars.iv56.unr to i32
  %conv8.epil = sitofp i32 %27 to double
  %mul9.epil = fmul fast double %conv8.epil, 0xC01921FB54442D18
  %28 = fmul fast double %mul9.epil, %3
  %29 = tail call fast double @llvm.cos.f64(double %28)
  %conv16.epil = fptrunc double %29 to float
  %r.epil = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv56.unr, i32 0
  store float %conv16.epil, float* %r.epil, align 4, !tbaa !10
  %30 = call fast double @llvm.sin.f64(double %28)
  %31 = fptrunc double %30 to float
  %conv17.epil = fsub fast float -0.000000e+00, %31
  %i22.epil = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 3, i64 %indvars.iv56.unr, i32 1
  store float %conv17.epil, float* %i22.epil, align 4, !tbaa !13
  br label %for.end

for.end:                                          ; preds = %for.body.epil, %for.end.loopexit62.unr-lcssa, %for.body.us.epil, %for.end.loopexit.unr-lcssa, %if.then5
  %arraydecay23 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st.1, i64 0, i32 2, i64 0
  %32 = tail call fast double @llvm.sqrt.f64(double %conv10) #7
  %33 = tail call fast double @llvm.floor.f64(double %32) #7
  br label %do.body.i

do.body.i:                                        ; preds = %while.end.i, %for.end
  %facbuf.addr.0.i = phi i32* [ %arraydecay23, %for.end ], [ %incdec.ptr3.i, %while.end.i ]
  %p.0.i = phi i32 [ 4, %for.end ], [ %p.1.lcssa.i, %while.end.i ]
  %n.addr.0.i = phi i32 [ %nfft, %for.end ], [ %div.i, %while.end.i ]
  %rem17.i = srem i32 %n.addr.0.i, %p.0.i
  %tobool18.i = icmp eq i32 %rem17.i, 0
  br i1 %tobool18.i, label %while.end.i, label %while.body.i

while.body.i:                                     ; preds = %do.body.i, %sw.epilog.i
  %p.119.i = phi i32 [ %p.3.i, %sw.epilog.i ], [ %p.0.i, %do.body.i ]
  switch i32 %p.119.i, label %sw.default.i [
    i32 4, label %sw.epilog.i
    i32 2, label %sw.bb1.i
  ]

sw.bb1.i:                                         ; preds = %while.body.i
  br label %sw.epilog.i

sw.default.i:                                     ; preds = %while.body.i
  %add.i = add nsw i32 %p.119.i, 2
  br label %sw.epilog.i

sw.epilog.i:                                      ; preds = %sw.default.i, %sw.bb1.i, %while.body.i
  %p.2.i = phi i32 [ %add.i, %sw.default.i ], [ 3, %sw.bb1.i ], [ 2, %while.body.i ]
  %conv2.i = sitofp i32 %p.2.i to double
  %cmp.i = fcmp fast olt double %33, %conv2.i
  %p.3.i = select i1 %cmp.i, i32 %n.addr.0.i, i32 %p.2.i
  %rem.i = srem i32 %n.addr.0.i, %p.3.i
  %tobool.i = icmp eq i32 %rem.i, 0
  br i1 %tobool.i, label %while.end.i, label %while.body.i

while.end.i:                                      ; preds = %sw.epilog.i, %do.body.i
  %p.1.lcssa.i = phi i32 [ %p.0.i, %do.body.i ], [ %p.3.i, %sw.epilog.i ]
  %div.i = sdiv i32 %n.addr.0.i, %p.1.lcssa.i
  %incdec.ptr.i = getelementptr inbounds i32, i32* %facbuf.addr.0.i, i64 1
  store i32 %p.1.lcssa.i, i32* %facbuf.addr.0.i, align 4, !tbaa !14
  %incdec.ptr3.i = getelementptr inbounds i32, i32* %facbuf.addr.0.i, i64 2
  store i32 %div.i, i32* %incdec.ptr.i, align 4, !tbaa !14
  %cmp4.i = icmp sgt i32 %div.i, 1
  br i1 %cmp4.i, label %do.body.i, label %if.end24

if.end24:                                         ; preds = %while.end.i, %if.end4
  ret %struct.kiss_fft_state* %st.1
}

; Function Attrs: nofree nounwind
declare dso_local noalias i8* @malloc(i64) local_unnamed_addr #1

; Function Attrs: nounwind readnone speculatable
declare double @llvm.cos.f64(double) #2

; Function Attrs: nounwind readnone speculatable
declare double @llvm.sin.f64(double) #2

; Function Attrs: nounwind uwtable
define dso_local void @kiss_fft_stride(%struct.kiss_fft_state* readonly %st, %struct.kiss_fft_cpx* readonly %fin, %struct.kiss_fft_cpx* %fout, i32 %in_stride) local_unnamed_addr #3 {
entry:
  %cmp = icmp eq %struct.kiss_fft_cpx* %fin, %fout
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %nfft = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 0
  %0 = load i32, i32* %nfft, align 4, !tbaa !6
  %conv = sext i32 %0 to i64
  %mul = shl nsw i64 %conv, 3
  %call = tail call noalias i8* @malloc(i64 %mul) #7
  %1 = bitcast i8* %call to %struct.kiss_fft_cpx*
  %arraydecay = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 2, i64 0
  tail call fastcc void @_ZL7kf_workP12kiss_fft_cpxPKS_miPiP14kiss_fft_state(%struct.kiss_fft_cpx* %1, %struct.kiss_fft_cpx* %fin, i64 1, i32 %in_stride, i32* nonnull %arraydecay, %struct.kiss_fft_state* %st)
  %2 = bitcast %struct.kiss_fft_cpx* %fin to i8*
  %3 = load i32, i32* %nfft, align 4, !tbaa !6
  %conv2 = sext i32 %3 to i64
  %mul3 = shl nsw i64 %conv2, 3
  tail call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %2, i8* align 4 %call, i64 %mul3, i1 false)
  tail call void @free(i8* %call) #7
  br label %if.end

if.else:                                          ; preds = %entry
  %arraydecay5 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 2, i64 0
  tail call fastcc void @_ZL7kf_workP12kiss_fft_cpxPKS_miPiP14kiss_fft_state(%struct.kiss_fft_cpx* %fout, %struct.kiss_fft_cpx* %fin, i64 1, i32 %in_stride, i32* nonnull %arraydecay5, %struct.kiss_fft_state* %st)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: nounwind uwtable
define internal fastcc void @_ZL7kf_workP12kiss_fft_cpxPKS_miPiP14kiss_fft_state(%struct.kiss_fft_cpx* %Fout, %struct.kiss_fft_cpx* nocapture readonly %f, i64 %fstride, i32 %in_stride, i32* nocapture readonly %factors, %struct.kiss_fft_state* readonly %st) unnamed_addr #3 {
entry:
  %incdec.ptr = getelementptr inbounds i32, i32* %factors, i64 1
  %0 = load i32, i32* %factors, align 4, !tbaa !14
  %incdec.ptr1 = getelementptr inbounds i32, i32* %factors, i64 2
  %1 = load i32, i32* %incdec.ptr, align 4, !tbaa !14
  %mul = mul nsw i32 %1, %0
  %idx.ext = sext i32 %mul to i64
  %add.ptr = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %idx.ext
  %cmp = icmp eq i32 %1, 1
  br i1 %cmp, label %do.body.preheader, label %do.body6.preheader

do.body6.preheader:                               ; preds = %entry
  %conv7 = sext i32 %0 to i64
  %mul8 = mul i64 %conv7, %fstride
  %conv9 = sext i32 %in_stride to i64
  %mul10 = mul i64 %conv9, %fstride
  %idx.ext13 = sext i32 %1 to i64
  br label %do.body6

do.body.preheader:                                ; preds = %entry
  %conv = sext i32 %in_stride to i64
  %mul2 = mul i64 %conv, %fstride
  %2 = shl nsw i64 %idx.ext, 3
  %3 = add nsw i64 %2, -8
  %4 = lshr exact i64 %3, 3
  %5 = add nuw nsw i64 %4, 1
  %xtraiter155 = and i64 %5, 7
  %lcmp.mod156 = icmp eq i64 %xtraiter155, 0
  br i1 %lcmp.mod156, label %do.body.prol.loopexit, label %do.body.prol

do.body.prol:                                     ; preds = %do.body.preheader, %do.body.prol
  %f.addr.0.prol = phi %struct.kiss_fft_cpx* [ %add.ptr3.prol, %do.body.prol ], [ %f, %do.body.preheader ]
  %Fout.addr.0.prol = phi %struct.kiss_fft_cpx* [ %incdec.ptr4.prol, %do.body.prol ], [ %Fout, %do.body.preheader ]
  %prol.iter = phi i64 [ %prol.iter.sub, %do.body.prol ], [ %xtraiter155, %do.body.preheader ]
  %6 = bitcast %struct.kiss_fft_cpx* %f.addr.0.prol to i64*
  %7 = bitcast %struct.kiss_fft_cpx* %Fout.addr.0.prol to i64*
  %8 = load i64, i64* %6, align 4
  store i64 %8, i64* %7, align 4
  %add.ptr3.prol = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %f.addr.0.prol, i64 %mul2
  %incdec.ptr4.prol = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.prol, i64 1
  %prol.iter.sub = add i64 %prol.iter, -1
  %prol.iter.cmp = icmp eq i64 %prol.iter.sub, 0
  br i1 %prol.iter.cmp, label %do.body.prol.loopexit, label %do.body.prol, !llvm.loop !15

do.body.prol.loopexit:                            ; preds = %do.body.prol, %do.body.preheader
  %f.addr.0.unr = phi %struct.kiss_fft_cpx* [ %f, %do.body.preheader ], [ %add.ptr3.prol, %do.body.prol ]
  %Fout.addr.0.unr = phi %struct.kiss_fft_cpx* [ %Fout, %do.body.preheader ], [ %incdec.ptr4.prol, %do.body.prol ]
  %9 = icmp ult i64 %3, 56
  br i1 %9, label %if.end, label %do.body

do.body:                                          ; preds = %do.body.prol.loopexit, %do.body
  %f.addr.0 = phi %struct.kiss_fft_cpx* [ %add.ptr3.7, %do.body ], [ %f.addr.0.unr, %do.body.prol.loopexit ]
  %Fout.addr.0 = phi %struct.kiss_fft_cpx* [ %incdec.ptr4.7, %do.body ], [ %Fout.addr.0.unr, %do.body.prol.loopexit ]
  %10 = bitcast %struct.kiss_fft_cpx* %f.addr.0 to i64*
  %11 = bitcast %struct.kiss_fft_cpx* %Fout.addr.0 to i64*
  %12 = load i64, i64* %10, align 4
  store i64 %12, i64* %11, align 4
  %add.ptr3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %f.addr.0, i64 %mul2
  %incdec.ptr4 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 1
  %13 = bitcast %struct.kiss_fft_cpx* %add.ptr3 to i64*
  %14 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4 to i64*
  %15 = load i64, i64* %13, align 4
  store i64 %15, i64* %14, align 4
  %add.ptr3.1 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3, i64 %mul2
  %incdec.ptr4.1 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 2
  %16 = bitcast %struct.kiss_fft_cpx* %add.ptr3.1 to i64*
  %17 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4.1 to i64*
  %18 = load i64, i64* %16, align 4
  store i64 %18, i64* %17, align 4
  %add.ptr3.2 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3.1, i64 %mul2
  %incdec.ptr4.2 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 3
  %19 = bitcast %struct.kiss_fft_cpx* %add.ptr3.2 to i64*
  %20 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4.2 to i64*
  %21 = load i64, i64* %19, align 4
  store i64 %21, i64* %20, align 4
  %add.ptr3.3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3.2, i64 %mul2
  %incdec.ptr4.3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 4
  %22 = bitcast %struct.kiss_fft_cpx* %add.ptr3.3 to i64*
  %23 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4.3 to i64*
  %24 = load i64, i64* %22, align 4
  store i64 %24, i64* %23, align 4
  %add.ptr3.4 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3.3, i64 %mul2
  %incdec.ptr4.4 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 5
  %25 = bitcast %struct.kiss_fft_cpx* %add.ptr3.4 to i64*
  %26 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4.4 to i64*
  %27 = load i64, i64* %25, align 4
  store i64 %27, i64* %26, align 4
  %add.ptr3.5 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3.4, i64 %mul2
  %incdec.ptr4.5 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 6
  %28 = bitcast %struct.kiss_fft_cpx* %add.ptr3.5 to i64*
  %29 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4.5 to i64*
  %30 = load i64, i64* %28, align 4
  store i64 %30, i64* %29, align 4
  %add.ptr3.6 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3.5, i64 %mul2
  %incdec.ptr4.6 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 7
  %31 = bitcast %struct.kiss_fft_cpx* %add.ptr3.6 to i64*
  %32 = bitcast %struct.kiss_fft_cpx* %incdec.ptr4.6 to i64*
  %33 = load i64, i64* %31, align 4
  store i64 %33, i64* %32, align 4
  %add.ptr3.7 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %add.ptr3.6, i64 %mul2
  %incdec.ptr4.7 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0, i64 8
  %cmp5.7 = icmp eq %struct.kiss_fft_cpx* %incdec.ptr4.7, %add.ptr
  br i1 %cmp5.7, label %if.end, label %do.body

do.body6:                                         ; preds = %do.body6.preheader, %do.body6
  %f.addr.1 = phi %struct.kiss_fft_cpx* [ %add.ptr11, %do.body6 ], [ %f, %do.body6.preheader ]
  %Fout.addr.1 = phi %struct.kiss_fft_cpx* [ %add.ptr14, %do.body6 ], [ %Fout, %do.body6.preheader ]
  tail call fastcc void @_ZL7kf_workP12kiss_fft_cpxPKS_miPiP14kiss_fft_state(%struct.kiss_fft_cpx* %Fout.addr.1, %struct.kiss_fft_cpx* %f.addr.1, i64 %mul8, i32 %in_stride, i32* nonnull %incdec.ptr1, %struct.kiss_fft_state* %st)
  %add.ptr11 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %f.addr.1, i64 %mul10
  %add.ptr14 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.1, i64 %idx.ext13
  %cmp15 = icmp eq %struct.kiss_fft_cpx* %add.ptr14, %add.ptr
  br i1 %cmp15, label %if.end, label %do.body6

if.end:                                           ; preds = %do.body6, %do.body.prol.loopexit, %do.body
  switch i32 %0, label %sw.default [
    i32 2, label %sw.bb
    i32 3, label %sw.bb17
    i32 4, label %sw.bb19
    i32 5, label %sw.bb21
  ]

sw.bb:                                            ; preds = %if.end
  %arraydecay.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 0
  %idx.ext.i = sext i32 %1 to i64
  %add.ptr.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %idx.ext.i
  br label %do.body.i

do.body.i:                                        ; preds = %do.body.i, %sw.bb
  %Fout2.0.i = phi %struct.kiss_fft_cpx* [ %add.ptr.i, %sw.bb ], [ %incdec.ptr.i, %do.body.i ]
  %tw1.0.i = phi %struct.kiss_fft_cpx* [ %arraydecay.i, %sw.bb ], [ %add.ptr13.i, %do.body.i ]
  %m.addr.0.i = phi i32 [ %1, %sw.bb ], [ %dec.i, %do.body.i ]
  %Fout.addr.0.i = phi %struct.kiss_fft_cpx* [ %Fout, %sw.bb ], [ %incdec.ptr34.i, %do.body.i ]
  %r.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout2.0.i, i64 0, i32 0
  %34 = load float, float* %r.i, align 4, !tbaa !10
  %r2.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i, i64 0, i32 0
  %35 = load float, float* %r2.i, align 4, !tbaa !10
  %mul.i66 = fmul fast float %35, %34
  %i.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout2.0.i, i64 0, i32 1
  %36 = load float, float* %i.i, align 4, !tbaa !13
  %i3.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i, i64 0, i32 1
  %37 = load float, float* %i3.i, align 4, !tbaa !13
  %mul4.i = fmul fast float %37, %36
  %sub.i = fsub fast float %mul.i66, %mul4.i
  %mul8.i = fmul fast float %37, %34
  %mul11.i = fmul fast float %36, %35
  %add.i = fadd fast float %mul8.i, %mul11.i
  %add.ptr13.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i, i64 %fstride
  %r15.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i, i64 0, i32 0
  %38 = load float, float* %r15.i, align 4, !tbaa !10
  %sub17.i = fsub fast float %38, %sub.i
  store float %sub17.i, float* %r.i, align 4, !tbaa !10
  %i19.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i, i64 0, i32 1
  %39 = load float, float* %i19.i, align 4, !tbaa !13
  %sub21.i = fsub fast float %39, %add.i
  store float %sub21.i, float* %i.i, align 4, !tbaa !13
  %40 = load float, float* %r15.i, align 4, !tbaa !10
  %add28.i = fadd fast float %40, %sub.i
  store float %add28.i, float* %r15.i, align 4, !tbaa !10
  %41 = load float, float* %i19.i, align 4, !tbaa !13
  %add31.i = fadd fast float %41, %add.i
  store float %add31.i, float* %i19.i, align 4, !tbaa !13
  %incdec.ptr.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout2.0.i, i64 1
  %incdec.ptr34.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i, i64 1
  %dec.i = add nsw i32 %m.addr.0.i, -1
  %tobool.i = icmp eq i32 %dec.i, 0
  br i1 %tobool.i, label %sw.epilog, label %do.body.i

sw.bb17:                                          ; preds = %if.end
  %conv18 = sext i32 %1 to i64
  %mul.i67 = shl nsw i64 %conv18, 1
  %mul1.i = mul i64 %conv18, %fstride
  %epi3.sroa.3.0..sroa_idx151.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul1.i, i32 1
  %epi3.sroa.3.0.copyload.i = load float, float* %epi3.sroa.3.0..sroa_idx151.i, align 4
  %arraydecay.i68 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 0
  %mul81.i = shl i64 %fstride, 1
  br label %do.body.i80

do.body.i80:                                      ; preds = %do.body.i80, %sw.bb17
  %tw2.0.i = phi %struct.kiss_fft_cpx* [ %arraydecay.i68, %sw.bb17 ], [ %add.ptr82.i, %do.body.i80 ]
  %tw1.0.i69 = phi %struct.kiss_fft_cpx* [ %arraydecay.i68, %sw.bb17 ], [ %add.ptr.i75, %do.body.i80 ]
  %k.0.i = phi i64 [ %conv18, %sw.bb17 ], [ %dec.i78, %do.body.i80 ]
  %Fout.addr.0.i70 = phi %struct.kiss_fft_cpx* [ %Fout, %sw.bb17 ], [ %incdec.ptr.i77, %do.body.i80 ]
  %r.i71 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 %conv18, i32 0
  %42 = load float, float* %r.i71, align 4, !tbaa !10
  %r5.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i69, i64 0, i32 0
  %43 = load float, float* %r5.i, align 4, !tbaa !10
  %mul6.i = fmul fast float %43, %42
  %i.i72 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 %conv18, i32 1
  %44 = load float, float* %i.i72, align 4, !tbaa !13
  %i8.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i69, i64 0, i32 1
  %45 = load float, float* %i8.i, align 4, !tbaa !13
  %mul9.i = fmul fast float %45, %44
  %sub.i73 = fsub fast float %mul6.i, %mul9.i
  %mul15.i = fmul fast float %45, %42
  %mul19.i = fmul fast float %44, %43
  %add.i74 = fadd fast float %mul15.i, %mul19.i
  %r24.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 %mul.i67, i32 0
  %46 = load float, float* %r24.i, align 4, !tbaa !10
  %r25.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.i, i64 0, i32 0
  %47 = load float, float* %r25.i, align 4, !tbaa !10
  %mul26.i = fmul fast float %47, %46
  %i28.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 %mul.i67, i32 1
  %48 = load float, float* %i28.i, align 4, !tbaa !13
  %i29.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.i, i64 0, i32 1
  %49 = load float, float* %i29.i, align 4, !tbaa !13
  %mul30.i = fmul fast float %49, %48
  %sub31.i = fsub fast float %mul26.i, %mul30.i
  %mul37.i = fmul fast float %49, %46
  %mul41.i = fmul fast float %48, %47
  %add42.i = fadd fast float %mul37.i, %mul41.i
  %add52.i = fadd fast float %sub31.i, %sub.i73
  %add59.i = fadd fast float %add42.i, %add.i74
  %sub69.i = fsub fast float %sub.i73, %sub31.i
  %sub76.i = fsub fast float %add.i74, %add42.i
  %add.ptr.i75 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i69, i64 %fstride
  %add.ptr82.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.i, i64 %mul81.i
  %r83.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 0, i32 0
  %50 = load float, float* %r83.i, align 4, !tbaa !10
  %conv.i76 = fpext float %50 to double
  %conv86.i = fpext float %add52.i to double
  %mul87.i = fmul fast double %conv86.i, 5.000000e-01
  %sub88.i = fsub fast double %conv.i76, %mul87.i
  %conv89.i = fptrunc double %sub88.i to float
  store float %conv89.i, float* %r.i71, align 4, !tbaa !10
  %i92.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 0, i32 1
  %51 = load float, float* %i92.i, align 4, !tbaa !13
  %conv93.i = fpext float %51 to double
  %conv96.i = fpext float %add59.i to double
  %mul97.i = fmul fast double %conv96.i, 5.000000e-01
  %sub98.i = fsub fast double %conv93.i, %mul97.i
  %conv99.i = fptrunc double %sub98.i to float
  store float %conv99.i, float* %i.i72, align 4, !tbaa !13
  %mul106.i = fmul fast float %sub69.i, %epi3.sroa.3.0.copyload.i
  %mul110.i = fmul fast float %sub76.i, %epi3.sroa.3.0.copyload.i
  %52 = load float, float* %r83.i, align 4, !tbaa !10
  %add117.i = fadd fast float %52, %add52.i
  store float %add117.i, float* %r83.i, align 4, !tbaa !10
  %53 = load float, float* %i92.i, align 4, !tbaa !13
  %add121.i = fadd fast float %53, %add59.i
  store float %add121.i, float* %i92.i, align 4, !tbaa !13
  %54 = load float, float* %r.i71, align 4, !tbaa !10
  %add128.i = fadd fast float %54, %mul110.i
  store float %add128.i, float* %r24.i, align 4, !tbaa !10
  %55 = load float, float* %i.i72, align 4, !tbaa !13
  %sub135.i = fsub fast float %55, %mul106.i
  store float %sub135.i, float* %i28.i, align 4, !tbaa !13
  %56 = load float, float* %r.i71, align 4, !tbaa !10
  %sub142.i = fsub fast float %56, %mul110.i
  store float %sub142.i, float* %r.i71, align 4, !tbaa !10
  %57 = load float, float* %i.i72, align 4, !tbaa !13
  %add147.i = fadd fast float %57, %mul106.i
  store float %add147.i, float* %i.i72, align 4, !tbaa !13
  %incdec.ptr.i77 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i70, i64 1
  %dec.i78 = add i64 %k.0.i, -1
  %tobool.i79 = icmp eq i64 %dec.i78, 0
  br i1 %tobool.i79, label %sw.epilog, label %do.body.i80

sw.bb19:                                          ; preds = %if.end
  %conv20 = sext i32 %1 to i64
  %mul.i81 = shl nsw i64 %conv20, 1
  %mul1.i82 = mul nsw i64 %conv20, 3
  %arraydecay.i83 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 0
  %mul145.i = shl i64 %fstride, 1
  %mul147.i = mul i64 %fstride, 3
  %inverse.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 1
  %58 = load i32, i32* %inverse.i, align 4, !tbaa !9
  %tobool.i84 = icmp eq i32 %58, 0
  br i1 %tobool.i84, label %do.body.us.i, label %do.body.i97

do.body.us.i:                                     ; preds = %sw.bb19, %do.body.us.i
  %tw3.0.us.i = phi %struct.kiss_fft_cpx* [ %add.ptr148.us.i, %do.body.us.i ], [ %arraydecay.i83, %sw.bb19 ]
  %tw2.0.us.i = phi %struct.kiss_fft_cpx* [ %add.ptr146.us.i, %do.body.us.i ], [ %arraydecay.i83, %sw.bb19 ]
  %k.0.us.i = phi i64 [ %dec.us.i, %do.body.us.i ], [ %conv20, %sw.bb19 ]
  %tw1.0.us.i = phi %struct.kiss_fft_cpx* [ %add.ptr.us.i, %do.body.us.i ], [ %arraydecay.i83, %sw.bb19 ]
  %Fout.addr.0.us.i = phi %struct.kiss_fft_cpx* [ %incdec.ptr.us.i, %do.body.us.i ], [ %Fout, %sw.bb19 ]
  %r.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 %conv20, i32 0
  %59 = load float, float* %r.us.i, align 4, !tbaa !10
  %r3.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.us.i, i64 0, i32 0
  %60 = load float, float* %r3.us.i, align 4, !tbaa !10
  %mul4.us.i = fmul fast float %60, %59
  %i.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 %conv20, i32 1
  %61 = load float, float* %i.us.i, align 4, !tbaa !13
  %i6.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.us.i, i64 0, i32 1
  %62 = load float, float* %i6.us.i, align 4, !tbaa !13
  %mul7.us.i = fmul fast float %62, %61
  %sub.us.i = fsub fast float %mul4.us.i, %mul7.us.i
  %mul13.us.i = fmul fast float %62, %59
  %mul17.us.i = fmul fast float %61, %60
  %add.us.i = fadd fast float %mul13.us.i, %mul17.us.i
  %r22.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 %mul.i81, i32 0
  %63 = load float, float* %r22.us.i, align 4, !tbaa !10
  %r23.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.us.i, i64 0, i32 0
  %64 = load float, float* %r23.us.i, align 4, !tbaa !10
  %mul24.us.i = fmul fast float %64, %63
  %i26.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 %mul.i81, i32 1
  %65 = load float, float* %i26.us.i, align 4, !tbaa !13
  %i27.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.us.i, i64 0, i32 1
  %66 = load float, float* %i27.us.i, align 4, !tbaa !13
  %mul28.us.i = fmul fast float %66, %65
  %sub29.us.i = fsub fast float %mul24.us.i, %mul28.us.i
  %mul35.us.i = fmul fast float %66, %63
  %mul39.us.i = fmul fast float %65, %64
  %add40.us.i = fadd fast float %mul35.us.i, %mul39.us.i
  %r47.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 %mul1.i82, i32 0
  %67 = load float, float* %r47.us.i, align 4, !tbaa !10
  %r48.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw3.0.us.i, i64 0, i32 0
  %68 = load float, float* %r48.us.i, align 4, !tbaa !10
  %mul49.us.i = fmul fast float %68, %67
  %i51.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 %mul1.i82, i32 1
  %69 = load float, float* %i51.us.i, align 4, !tbaa !13
  %i52.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw3.0.us.i, i64 0, i32 1
  %70 = load float, float* %i52.us.i, align 4, !tbaa !13
  %mul53.us.i = fmul fast float %70, %69
  %sub54.us.i = fsub fast float %mul49.us.i, %mul53.us.i
  %mul60.us.i = fmul fast float %70, %67
  %mul64.us.i = fmul fast float %69, %68
  %add65.us.i = fadd fast float %mul60.us.i, %mul64.us.i
  %r71.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 0, i32 0
  %71 = load float, float* %r71.us.i, align 4, !tbaa !10
  %sub74.us.i = fsub fast float %71, %sub29.us.i
  %i77.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 0, i32 1
  %72 = load float, float* %i77.us.i, align 4, !tbaa !13
  %sub80.us.i = fsub fast float %72, %add40.us.i
  %add89.us.i = fadd fast float %71, %sub29.us.i
  store float %add89.us.i, float* %r71.us.i, align 4, !tbaa !10
  %add93.us.i = fadd fast float %72, %add40.us.i
  store float %add93.us.i, float* %i77.us.i, align 4, !tbaa !13
  %add101.us.i = fadd fast float %sub54.us.i, %sub.us.i
  %add108.us.i = fadd fast float %add65.us.i, %add.us.i
  %sub118.us.i = fsub fast float %sub.us.i, %sub54.us.i
  %sub125.us.i = fsub fast float %add.us.i, %add65.us.i
  %sub134.us.i = fsub fast float %add89.us.i, %add101.us.i
  store float %sub134.us.i, float* %r22.us.i, align 4, !tbaa !10
  %sub140.us.i = fsub fast float %add93.us.i, %add108.us.i
  store float %sub140.us.i, float* %i26.us.i, align 4, !tbaa !13
  %add.ptr.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.us.i, i64 %fstride
  %add.ptr146.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.us.i, i64 %mul145.i
  %add.ptr148.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw3.0.us.i, i64 %mul147.i
  %73 = load float, float* %r71.us.i, align 4, !tbaa !10
  %add153.us.i = fadd fast float %73, %add101.us.i
  store float %add153.us.i, float* %r71.us.i, align 4, !tbaa !10
  %74 = load float, float* %i77.us.i, align 4, !tbaa !13
  %add157.us.i = fadd fast float %74, %add108.us.i
  store float %add157.us.i, float* %i77.us.i, align 4, !tbaa !13
  %add192.us.i = fadd fast float %sub125.us.i, %sub74.us.i
  store float %add192.us.i, float* %r.us.i, align 4, !tbaa !10
  %sub199.us.i = fsub fast float %sub80.us.i, %sub118.us.i
  store float %sub199.us.i, float* %i.us.i, align 4, !tbaa !13
  %sub206.us.i = fsub fast float %sub74.us.i, %sub125.us.i
  store float %sub206.us.i, float* %r47.us.i, align 4, !tbaa !10
  %add213.us.i = fadd fast float %sub80.us.i, %sub118.us.i
  store float %add213.us.i, float* %i51.us.i, align 4, !tbaa !13
  %incdec.ptr.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.us.i, i64 1
  %dec.us.i = add i64 %k.0.us.i, -1
  %tobool217.us.i = icmp eq i64 %dec.us.i, 0
  br i1 %tobool217.us.i, label %sw.epilog, label %do.body.us.i

do.body.i97:                                      ; preds = %sw.bb19, %do.body.i97
  %tw3.0.i = phi %struct.kiss_fft_cpx* [ %add.ptr148.i, %do.body.i97 ], [ %arraydecay.i83, %sw.bb19 ]
  %tw2.0.i85 = phi %struct.kiss_fft_cpx* [ %add.ptr146.i, %do.body.i97 ], [ %arraydecay.i83, %sw.bb19 ]
  %k.0.i86 = phi i64 [ %dec.i96, %do.body.i97 ], [ %conv20, %sw.bb19 ]
  %tw1.0.i87 = phi %struct.kiss_fft_cpx* [ %add.ptr.i94, %do.body.i97 ], [ %arraydecay.i83, %sw.bb19 ]
  %Fout.addr.0.i88 = phi %struct.kiss_fft_cpx* [ %incdec.ptr.i95, %do.body.i97 ], [ %Fout, %sw.bb19 ]
  %r.i89 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 %conv20, i32 0
  %75 = load float, float* %r.i89, align 4, !tbaa !10
  %r3.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i87, i64 0, i32 0
  %76 = load float, float* %r3.i, align 4, !tbaa !10
  %mul4.i90 = fmul fast float %76, %75
  %i.i91 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 %conv20, i32 1
  %77 = load float, float* %i.i91, align 4, !tbaa !13
  %i6.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i87, i64 0, i32 1
  %78 = load float, float* %i6.i, align 4, !tbaa !13
  %mul7.i = fmul fast float %78, %77
  %sub.i92 = fsub fast float %mul4.i90, %mul7.i
  %mul13.i = fmul fast float %78, %75
  %mul17.i = fmul fast float %77, %76
  %add.i93 = fadd fast float %mul13.i, %mul17.i
  %r22.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 %mul.i81, i32 0
  %79 = load float, float* %r22.i, align 4, !tbaa !10
  %r23.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.i85, i64 0, i32 0
  %80 = load float, float* %r23.i, align 4, !tbaa !10
  %mul24.i = fmul fast float %80, %79
  %i26.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 %mul.i81, i32 1
  %81 = load float, float* %i26.i, align 4, !tbaa !13
  %i27.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.i85, i64 0, i32 1
  %82 = load float, float* %i27.i, align 4, !tbaa !13
  %mul28.i = fmul fast float %82, %81
  %sub29.i = fsub fast float %mul24.i, %mul28.i
  %mul35.i = fmul fast float %82, %79
  %mul39.i = fmul fast float %81, %80
  %add40.i = fadd fast float %mul35.i, %mul39.i
  %r47.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 %mul1.i82, i32 0
  %83 = load float, float* %r47.i, align 4, !tbaa !10
  %r48.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw3.0.i, i64 0, i32 0
  %84 = load float, float* %r48.i, align 4, !tbaa !10
  %mul49.i = fmul fast float %84, %83
  %i51.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 %mul1.i82, i32 1
  %85 = load float, float* %i51.i, align 4, !tbaa !13
  %i52.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw3.0.i, i64 0, i32 1
  %86 = load float, float* %i52.i, align 4, !tbaa !13
  %mul53.i = fmul fast float %86, %85
  %sub54.i = fsub fast float %mul49.i, %mul53.i
  %mul60.i = fmul fast float %86, %83
  %mul64.i = fmul fast float %85, %84
  %add65.i = fadd fast float %mul60.i, %mul64.i
  %r71.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 0, i32 0
  %87 = load float, float* %r71.i, align 4, !tbaa !10
  %sub74.i = fsub fast float %87, %sub29.i
  %i77.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 0, i32 1
  %88 = load float, float* %i77.i, align 4, !tbaa !13
  %sub80.i = fsub fast float %88, %add40.i
  %add89.i = fadd fast float %87, %sub29.i
  store float %add89.i, float* %r71.i, align 4, !tbaa !10
  %add93.i = fadd fast float %88, %add40.i
  store float %add93.i, float* %i77.i, align 4, !tbaa !13
  %add101.i = fadd fast float %sub54.i, %sub.i92
  %add108.i = fadd fast float %add65.i, %add.i93
  %sub118.i = fsub fast float %sub.i92, %sub54.i
  %sub125.i = fsub fast float %add.i93, %add65.i
  %sub134.i = fsub fast float %add89.i, %add101.i
  store float %sub134.i, float* %r22.i, align 4, !tbaa !10
  %sub140.i = fsub fast float %add93.i, %add108.i
  store float %sub140.i, float* %i26.i, align 4, !tbaa !13
  %add.ptr.i94 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw1.0.i87, i64 %fstride
  %add.ptr146.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw2.0.i85, i64 %mul145.i
  %add.ptr148.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %tw3.0.i, i64 %mul147.i
  %89 = load float, float* %r71.i, align 4, !tbaa !10
  %add153.i = fadd fast float %89, %add101.i
  store float %add153.i, float* %r71.i, align 4, !tbaa !10
  %90 = load float, float* %i77.i, align 4, !tbaa !13
  %add157.i = fadd fast float %90, %add108.i
  store float %add157.i, float* %i77.i, align 4, !tbaa !13
  %sub164.i = fsub fast float %sub74.i, %sub125.i
  store float %sub164.i, float* %r.i89, align 4, !tbaa !10
  %add171.i = fadd fast float %sub80.i, %sub118.i
  store float %add171.i, float* %i.i91, align 4, !tbaa !13
  %add178.i = fadd fast float %sub125.i, %sub74.i
  store float %add178.i, float* %r47.i, align 4, !tbaa !10
  %sub185.i = fsub fast float %sub80.i, %sub118.i
  store float %sub185.i, float* %i51.i, align 4, !tbaa !13
  %incdec.ptr.i95 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout.addr.0.i88, i64 1
  %dec.i96 = add i64 %k.0.i86, -1
  %tobool217.i = icmp eq i64 %dec.i96, 0
  br i1 %tobool217.i, label %sw.epilog, label %do.body.i97

sw.bb21:                                          ; preds = %if.end
  %conv.i98 = sext i32 %1 to i64
  %mul.i99 = mul i64 %conv.i98, %fstride
  %ya.sroa.0.0..sroa_idx.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul.i99, i32 0
  %ya.sroa.0.0.copyload.i = load float, float* %ya.sroa.0.0..sroa_idx.i, align 4
  %ya.sroa.7.0..sroa_idx416.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul.i99, i32 1
  %ya.sroa.7.0.copyload.i = load float, float* %ya.sroa.7.0..sroa_idx416.i, align 4
  %mul2.i = shl i64 %fstride, 1
  %mul4.i100 = mul i64 %mul2.i, %conv.i98
  %yb.sroa.0.0..sroa_idx.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul4.i100, i32 0
  %yb.sroa.0.0.copyload.i = load float, float* %yb.sroa.0.0..sroa_idx.i, align 4
  %yb.sroa.7.0..sroa_idx407.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul4.i100, i32 1
  %yb.sroa.7.0.copyload.i = load float, float* %yb.sroa.7.0..sroa_idx407.i, align 4
  %cmp574.i = icmp sgt i32 %1, 0
  br i1 %cmp574.i, label %for.body.lr.ph.i, label %sw.epilog

for.body.lr.ph.i:                                 ; preds = %sw.bb21
  %mul12.i = shl nsw i32 %1, 2
  %idx.ext13.i = sext i32 %mul12.i to i64
  %add.ptr14.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %idx.ext13.i
  %mul9.i101 = mul nsw i32 %1, 3
  %idx.ext10.i = sext i32 %mul9.i101 to i64
  %add.ptr11.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %idx.ext10.i
  %mul6.i102 = shl nsw i32 %1, 1
  %idx.ext7.i = sext i32 %mul6.i102 to i64
  %add.ptr8.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %idx.ext7.i
  %add.ptr.i103 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %conv.i98
  %wide.trip.count.i104 = zext i32 %1 to i64
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %for.body.lr.ph.i
  %indvars.iv.i105 = phi i64 [ 0, %for.body.lr.ph.i ], [ %indvars.iv.next.i116, %for.body.i ]
  %Fout0.0580.i = phi %struct.kiss_fft_cpx* [ %Fout, %for.body.lr.ph.i ], [ %incdec.ptr.i115, %for.body.i ]
  %Fout1.0579.i = phi %struct.kiss_fft_cpx* [ %add.ptr.i103, %for.body.lr.ph.i ], [ %incdec.ptr399.i, %for.body.i ]
  %Fout2.0578.i = phi %struct.kiss_fft_cpx* [ %add.ptr8.i, %for.body.lr.ph.i ], [ %incdec.ptr400.i, %for.body.i ]
  %Fout3.0577.i = phi %struct.kiss_fft_cpx* [ %add.ptr11.i, %for.body.lr.ph.i ], [ %incdec.ptr401.i, %for.body.i ]
  %Fout4.0576.i = phi %struct.kiss_fft_cpx* [ %add.ptr14.i, %for.body.lr.ph.i ], [ %incdec.ptr402.i, %for.body.i ]
  %scratch.sroa.0.0..sroa_idx.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout0.0580.i, i64 0, i32 0
  %scratch.sroa.0.0.copyload.i = load float, float* %scratch.sroa.0.0..sroa_idx.i, align 4
  %scratch.sroa.5.0..sroa_idx442.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout0.0580.i, i64 0, i32 1
  %scratch.sroa.5.0.copyload.i = load float, float* %scratch.sroa.5.0..sroa_idx442.i, align 4
  %r.i106 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout1.0579.i, i64 0, i32 0
  %91 = load float, float* %r.i106, align 4, !tbaa !10
  %mul19.i107 = mul i64 %indvars.iv.i105, %fstride
  %r21.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul19.i107, i32 0
  %92 = load float, float* %r21.i, align 4, !tbaa !10
  %mul22.i = fmul fast float %92, %91
  %i.i108 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout1.0579.i, i64 0, i32 1
  %93 = load float, float* %i.i108, align 4, !tbaa !13
  %i26.i109 = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul19.i107, i32 1
  %94 = load float, float* %i26.i109, align 4, !tbaa !13
  %mul27.i = fmul fast float %94, %93
  %sub.i110 = fsub fast float %mul22.i, %mul27.i
  %mul35.i111 = fmul fast float %94, %91
  %mul41.i112 = fmul fast float %93, %92
  %add.i113 = fadd fast float %mul35.i111, %mul41.i112
  %r45.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout2.0578.i, i64 0, i32 0
  %95 = load float, float* %r45.i, align 4, !tbaa !10
  %96 = shl nuw nsw i64 %indvars.iv.i105, 1
  %mul48.i = mul i64 %96, %fstride
  %r50.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul48.i, i32 0
  %97 = load float, float* %r50.i, align 4, !tbaa !10
  %mul51.i = fmul fast float %97, %95
  %i52.i114 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout2.0578.i, i64 0, i32 1
  %98 = load float, float* %i52.i114, align 4, !tbaa !13
  %i57.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul48.i, i32 1
  %99 = load float, float* %i57.i, align 4, !tbaa !13
  %mul58.i = fmul fast float %99, %98
  %sub59.i = fsub fast float %mul51.i, %mul58.i
  %mul68.i = fmul fast float %99, %95
  %mul75.i = fmul fast float %98, %97
  %add76.i = fadd fast float %mul68.i, %mul75.i
  %r82.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout3.0577.i, i64 0, i32 0
  %100 = load float, float* %r82.i, align 4, !tbaa !10
  %mul83.i = mul i64 %indvars.iv.i105, 3
  %conv84.i = and i64 %mul83.i, 4294967295
  %mul85.i = mul i64 %conv84.i, %fstride
  %r87.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul85.i, i32 0
  %101 = load float, float* %r87.i, align 4, !tbaa !10
  %mul88.i = fmul fast float %101, %100
  %i89.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout3.0577.i, i64 0, i32 1
  %102 = load float, float* %i89.i, align 4, !tbaa !13
  %i94.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul85.i, i32 1
  %103 = load float, float* %i94.i, align 4, !tbaa !13
  %mul95.i = fmul fast float %103, %102
  %sub96.i = fsub fast float %mul88.i, %mul95.i
  %mul105.i = fmul fast float %103, %100
  %mul112.i = fmul fast float %102, %101
  %add113.i = fadd fast float %mul105.i, %mul112.i
  %r119.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout4.0576.i, i64 0, i32 0
  %104 = load float, float* %r119.i, align 4, !tbaa !10
  %mul120.i = shl i64 %indvars.iv.i105, 2
  %conv121.i = and i64 %mul120.i, 4294967292
  %mul122.i = mul i64 %conv121.i, %fstride
  %r124.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul122.i, i32 0
  %105 = load float, float* %r124.i, align 4, !tbaa !10
  %mul125.i = fmul fast float %105, %104
  %i126.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout4.0576.i, i64 0, i32 1
  %106 = load float, float* %i126.i, align 4, !tbaa !13
  %i131.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %mul122.i, i32 1
  %107 = load float, float* %i131.i, align 4, !tbaa !13
  %mul132.i = fmul fast float %107, %106
  %sub133.i = fsub fast float %mul125.i, %mul132.i
  %mul142.i = fmul fast float %107, %104
  %mul149.i = fmul fast float %106, %105
  %add150.i = fadd fast float %mul142.i, %mul149.i
  %add160.i = fadd fast float %sub133.i, %sub.i110
  %add167.i = fadd fast float %add150.i, %add.i113
  %sub177.i = fsub fast float %sub.i110, %sub133.i
  %sub184.i = fsub fast float %add.i113, %add150.i
  %add194.i = fadd fast float %sub96.i, %sub59.i
  %add201.i = fadd fast float %add113.i, %add76.i
  %sub211.i = fsub fast float %sub59.i, %sub96.i
  %sub218.i = fsub fast float %add76.i, %add113.i
  %add227.i = fadd fast float %add194.i, %scratch.sroa.0.0.copyload.i
  %add229.i = fadd fast float %add227.i, %add160.i
  store float %add229.i, float* %scratch.sroa.0.0..sroa_idx.i, align 4, !tbaa !10
  %add234.i = fadd fast float %add201.i, %scratch.sroa.5.0.copyload.i
  %add236.i = fadd fast float %add234.i, %add167.i
  store float %add236.i, float* %scratch.sroa.5.0..sroa_idx442.i, align 4, !tbaa !13
  %mul242.i = fmul fast float %add160.i, %ya.sroa.0.0.copyload.i
  %mul247.i = fmul fast float %add194.i, %yb.sroa.0.0.copyload.i
  %add243.i = fadd fast float %mul247.i, %scratch.sroa.0.0.copyload.i
  %add248.i = fadd fast float %add243.i, %mul242.i
  %mul256.i = fmul fast float %add167.i, %ya.sroa.0.0.copyload.i
  %mul261.i = fmul fast float %add201.i, %yb.sroa.0.0.copyload.i
  %add257.i = fadd fast float %mul261.i, %scratch.sroa.5.0.copyload.i
  %add262.i = fadd fast float %add257.i, %mul256.i
  %mul268.i = fmul fast float %sub184.i, %ya.sroa.7.0.copyload.i
  %mul272.i = fmul fast float %sub218.i, %yb.sroa.7.0.copyload.i
  %add273.i = fadd fast float %mul268.i, %mul272.i
  %108 = fmul fast float %ya.sroa.7.0.copyload.i, %sub177.i
  %sub280.i = fsub fast float -0.000000e+00, %108
  %mul284.i = fmul fast float %sub211.i, %yb.sroa.7.0.copyload.i
  %sub285.i = fsub fast float %sub280.i, %mul284.i
  %sub293.i = fsub fast float %add248.i, %add273.i
  store float %sub293.i, float* %r.i106, align 4, !tbaa !10
  %sub299.i = fsub fast float %add262.i, %sub285.i
  store float %sub299.i, float* %i.i108, align 4, !tbaa !13
  %add308.i = fadd fast float %add248.i, %add273.i
  store float %add308.i, float* %r119.i, align 4, !tbaa !10
  %add314.i = fadd fast float %add262.i, %sub285.i
  store float %add314.i, float* %i126.i, align 4, !tbaa !13
  %mul323.i = fmul fast float %add160.i, %yb.sroa.0.0.copyload.i
  %mul328.i = fmul fast float %add194.i, %ya.sroa.0.0.copyload.i
  %add324.i = fadd fast float %mul328.i, %scratch.sroa.0.0.copyload.i
  %add329.i = fadd fast float %add324.i, %mul323.i
  %mul337.i = fmul fast float %add167.i, %yb.sroa.0.0.copyload.i
  %mul342.i = fmul fast float %add201.i, %ya.sroa.0.0.copyload.i
  %add338.i = fadd fast float %mul342.i, %scratch.sroa.5.0.copyload.i
  %add343.i = fadd fast float %add338.i, %mul337.i
  %mul349.i = fmul fast float %sub184.i, %yb.sroa.7.0.copyload.i
  %mul354.i = fmul fast float %sub218.i, %ya.sroa.7.0.copyload.i
  %add355.i = fsub fast float %mul354.i, %mul349.i
  %mul361.i = fmul fast float %sub177.i, %yb.sroa.7.0.copyload.i
  %mul365.i = fmul fast float %sub211.i, %ya.sroa.7.0.copyload.i
  %sub366.i = fsub fast float %mul361.i, %mul365.i
  %add374.i = fadd fast float %add329.i, %add355.i
  store float %add374.i, float* %r45.i, align 4, !tbaa !10
  %add380.i = fadd fast float %add343.i, %sub366.i
  store float %add380.i, float* %i52.i114, align 4, !tbaa !13
  %sub389.i = fsub fast float %add329.i, %add355.i
  store float %sub389.i, float* %r82.i, align 4, !tbaa !10
  %sub395.i = fsub fast float %add343.i, %sub366.i
  store float %sub395.i, float* %i89.i, align 4, !tbaa !13
  %incdec.ptr.i115 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout0.0580.i, i64 1
  %incdec.ptr399.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout1.0579.i, i64 1
  %incdec.ptr400.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout2.0578.i, i64 1
  %incdec.ptr401.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout3.0577.i, i64 1
  %incdec.ptr402.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout4.0576.i, i64 1
  %indvars.iv.next.i116 = add nuw nsw i64 %indvars.iv.i105, 1
  %exitcond.i117 = icmp eq i64 %indvars.iv.next.i116, %wide.trip.count.i104
  br i1 %exitcond.i117, label %sw.epilog, label %for.body.i

sw.default:                                       ; preds = %if.end
  %nfft.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 0
  %109 = load i32, i32* %nfft.i, align 4, !tbaa !6
  %conv.i = sext i32 %0 to i64
  %mul.i = shl nsw i64 %conv.i, 3
  %call.i = tail call noalias i8* @malloc(i64 %mul.i) #7
  %110 = bitcast i8* %call.i to %struct.kiss_fft_cpx*
  %cmp139.i = icmp sgt i32 %1, 0
  br i1 %cmp139.i, label %for.cond2.preheader.lr.ph.i, label %_ZL15kf_bfly_genericP12kiss_fft_cpxmP14kiss_fft_stateii.exit

for.cond2.preheader.lr.ph.i:                      ; preds = %sw.default
  %cmp3129.i = icmp sgt i32 %0, 0
  %111 = bitcast i8* %call.i to i64*
  %112 = trunc i64 %fstride to i32
  br i1 %cmp3129.i, label %for.cond2.preheader.us.preheader.i, label %_ZL15kf_bfly_genericP12kiss_fft_cpxmP14kiss_fft_stateii.exit

for.cond2.preheader.us.preheader.i:               ; preds = %for.cond2.preheader.lr.ph.i
  %cmp14132.i = icmp eq i32 %0, 1
  %113 = sext i32 %1 to i64
  %wide.trip.count200.i = zext i32 %1 to i64
  %wide.trip.count.i = zext i32 %0 to i64
  %114 = add nsw i64 %wide.trip.count.i, -1
  %xtraiter = and i64 %wide.trip.count.i, 3
  %115 = icmp ult i64 %114, 3
  %unroll_iter = sub nsw i64 %wide.trip.count.i, %xtraiter
  %lcmp.mod = icmp eq i64 %xtraiter, 0
  br i1 %cmp14132.i, label %for.cond2.preheader.us.i, label %for.cond2.preheader.us.i.us

for.cond2.preheader.us.i.us:                      ; preds = %for.cond2.preheader.us.preheader.i, %for.inc72.us.i.loopexit.us
  %indvars.iv.i.us = phi i64 [ %indvars.iv.next.i.us, %for.inc72.us.i.loopexit.us ], [ 0, %for.cond2.preheader.us.preheader.i ]
  br i1 %115, label %for.body9.lr.ph.us.i.us.unr-lcssa, label %for.body4.us.i.us

for.body4.us.i.us:                                ; preds = %for.cond2.preheader.us.i.us, %for.body4.us.i.us
  %indvars.iv188.i.us = phi i64 [ %indvars.iv.next189.i.us.3, %for.body4.us.i.us ], [ 0, %for.cond2.preheader.us.i.us ]
  %indvars.iv186.i.us = phi i64 [ %indvars.iv.next187.i.us.3, %for.body4.us.i.us ], [ %indvars.iv.i.us, %for.cond2.preheader.us.i.us ]
  %niter154 = phi i64 [ %niter154.nsub.3, %for.body4.us.i.us ], [ %unroll_iter, %for.cond2.preheader.us.i.us ]
  %arrayidx.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv186.i.us
  %arrayidx6.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv188.i.us
  %116 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.us to i64*
  %117 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.us to i64*
  %118 = load i64, i64* %116, align 4
  store i64 %118, i64* %117, align 4
  %indvars.iv.next187.i.us = add nsw i64 %indvars.iv186.i.us, %113
  %indvars.iv.next189.i.us = or i64 %indvars.iv188.i.us, 1
  %arrayidx.us.i.us.1 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.next187.i.us
  %arrayidx6.us.i.us.1 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv.next189.i.us
  %119 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.us.1 to i64*
  %120 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.us.1 to i64*
  %121 = load i64, i64* %119, align 4
  store i64 %121, i64* %120, align 4
  %indvars.iv.next187.i.us.1 = add nsw i64 %indvars.iv.next187.i.us, %113
  %indvars.iv.next189.i.us.1 = or i64 %indvars.iv188.i.us, 2
  %arrayidx.us.i.us.2 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.next187.i.us.1
  %arrayidx6.us.i.us.2 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv.next189.i.us.1
  %122 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.us.2 to i64*
  %123 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.us.2 to i64*
  %124 = load i64, i64* %122, align 4
  store i64 %124, i64* %123, align 4
  %indvars.iv.next187.i.us.2 = add nsw i64 %indvars.iv.next187.i.us.1, %113
  %indvars.iv.next189.i.us.2 = or i64 %indvars.iv188.i.us, 3
  %arrayidx.us.i.us.3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.next187.i.us.2
  %arrayidx6.us.i.us.3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv.next189.i.us.2
  %125 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.us.3 to i64*
  %126 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.us.3 to i64*
  %127 = load i64, i64* %125, align 4
  store i64 %127, i64* %126, align 4
  %indvars.iv.next187.i.us.3 = add nsw i64 %indvars.iv.next187.i.us.2, %113
  %indvars.iv.next189.i.us.3 = add nuw nsw i64 %indvars.iv188.i.us, 4
  %niter154.nsub.3 = add i64 %niter154, -4
  %niter154.ncmp.3 = icmp eq i64 %niter154.nsub.3, 0
  br i1 %niter154.ncmp.3, label %for.body9.lr.ph.us.i.us.unr-lcssa, label %for.body4.us.i.us

for.body9.lr.ph.us.i.us.unr-lcssa:                ; preds = %for.body4.us.i.us, %for.cond2.preheader.us.i.us
  %indvars.iv188.i.us.unr = phi i64 [ 0, %for.cond2.preheader.us.i.us ], [ %indvars.iv.next189.i.us.3, %for.body4.us.i.us ]
  %indvars.iv186.i.us.unr = phi i64 [ %indvars.iv.i.us, %for.cond2.preheader.us.i.us ], [ %indvars.iv.next187.i.us.3, %for.body4.us.i.us ]
  br i1 %lcmp.mod, label %for.body9.lr.ph.us.i.us, label %for.body4.us.i.us.epil

for.body4.us.i.us.epil:                           ; preds = %for.body9.lr.ph.us.i.us.unr-lcssa, %for.body4.us.i.us.epil
  %indvars.iv188.i.us.epil = phi i64 [ %indvars.iv.next189.i.us.epil, %for.body4.us.i.us.epil ], [ %indvars.iv188.i.us.unr, %for.body9.lr.ph.us.i.us.unr-lcssa ]
  %indvars.iv186.i.us.epil = phi i64 [ %indvars.iv.next187.i.us.epil, %for.body4.us.i.us.epil ], [ %indvars.iv186.i.us.unr, %for.body9.lr.ph.us.i.us.unr-lcssa ]
  %epil.iter151 = phi i64 [ %epil.iter151.sub, %for.body4.us.i.us.epil ], [ %xtraiter, %for.body9.lr.ph.us.i.us.unr-lcssa ]
  %arrayidx.us.i.us.epil = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv186.i.us.epil
  %arrayidx6.us.i.us.epil = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv188.i.us.epil
  %128 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.us.epil to i64*
  %129 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.us.epil to i64*
  %130 = load i64, i64* %128, align 4
  store i64 %130, i64* %129, align 4
  %indvars.iv.next187.i.us.epil = add nsw i64 %indvars.iv186.i.us.epil, %113
  %indvars.iv.next189.i.us.epil = add nuw nsw i64 %indvars.iv188.i.us.epil, 1
  %epil.iter151.sub = add i64 %epil.iter151, -1
  %epil.iter151.cmp = icmp eq i64 %epil.iter151.sub, 0
  br i1 %epil.iter151.cmp, label %for.body9.lr.ph.us.i.us, label %for.body4.us.i.us.epil, !llvm.loop !17

for.body9.lr.ph.us.i.us:                          ; preds = %for.body4.us.i.us.epil, %for.body9.lr.ph.us.i.us.unr-lcssa
  %131 = load i64, i64* %111, align 4
  %132 = trunc i64 %131 to i32
  %133 = bitcast i32 %132 to float
  br label %for.body9.us.us.i.us

for.body9.us.us.i.us:                             ; preds = %for.cond13.for.end67_crit_edge.us.us.i.us, %for.body9.lr.ph.us.i.us
  %indvars.iv197.i.us = phi i64 [ %indvars.iv.i.us, %for.body9.lr.ph.us.i.us ], [ %indvars.iv.next198.i.us, %for.cond13.for.end67_crit_edge.us.us.i.us ]
  %q1.1136.us.us.i.us = phi i32 [ 0, %for.body9.lr.ph.us.i.us ], [ %inc70.us.us.i.us, %for.cond13.for.end67_crit_edge.us.us.i.us ]
  %arrayidx12.us.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv197.i.us
  %134 = bitcast %struct.kiss_fft_cpx* %arrayidx12.us.us.i.us to i64*
  store i64 %131, i64* %134, align 4
  %135 = trunc i64 %indvars.iv197.i.us to i32
  %136 = mul i32 %135, %112
  %r56.us.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %arrayidx12.us.us.i.us, i64 0, i32 0
  %i61.us.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv197.i.us, i32 1
  %.pre.i.us = load float, float* %i61.us.us.i.us, align 4, !tbaa !13
  br label %for.body15.us.us.i.us

for.body15.us.us.i.us:                            ; preds = %for.body15.us.us.i.us, %for.body9.us.us.i.us
  %137 = phi float [ %add62.us.us.i.us, %for.body15.us.us.i.us ], [ %.pre.i.us, %for.body9.us.us.i.us ]
  %138 = phi float [ %add57.us.us.i.us, %for.body15.us.us.i.us ], [ %133, %for.body9.us.us.i.us ]
  %indvars.iv193.i.us = phi i64 [ %indvars.iv.next194.i.us, %for.body15.us.us.i.us ], [ 1, %for.body9.us.us.i.us ]
  %twidx.0134.us.us.i.us = phi i32 [ %spec.select.us.us.i.us, %for.body15.us.us.i.us ], [ 0, %for.body9.us.us.i.us ]
  %conv20.us.us.i.us = add i32 %twidx.0134.us.us.i.us, %136
  %cmp21.us.us.i.us = icmp slt i32 %conv20.us.us.i.us, %109
  %sub.us.us.i.us = select i1 %cmp21.us.us.i.us, i32 0, i32 %109
  %spec.select.us.us.i.us = sub nsw i32 %conv20.us.us.i.us, %sub.us.us.i.us
  %r.us.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv193.i.us, i32 0
  %139 = load float, float* %r.us.us.i.us, align 4, !tbaa !10
  %idxprom24.us.us.i.us = sext i32 %spec.select.us.us.i.us to i64
  %r26.us.us.i.us = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %idxprom24.us.us.i.us, i32 0
  %140 = load float, float* %r26.us.us.i.us, align 4, !tbaa !10
  %mul27.us.us.i.us = fmul fast float %140, %139
  %i.us.us.i.us = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv193.i.us, i32 1
  %141 = load float, float* %i.us.us.i.us, align 4, !tbaa !13
  %i32.us.us.i.us = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %st, i64 0, i32 3, i64 %idxprom24.us.us.i.us, i32 1
  %142 = load float, float* %i32.us.us.i.us, align 4, !tbaa !13
  %mul33.us.us.i.us = fmul fast float %142, %141
  %mul42.us.us.i.us = fmul fast float %142, %139
  %mul49.us.us.i.us = fmul fast float %141, %140
  %sub34.us.us.i.us = fadd fast float %mul27.us.us.i.us, %138
  %add57.us.us.i.us = fsub fast float %sub34.us.us.i.us, %mul33.us.us.i.us
  store float %add57.us.us.i.us, float* %r56.us.us.i.us, align 4, !tbaa !10
  %add50.us.us.i.us = fadd fast float %mul49.us.us.i.us, %137
  %add62.us.us.i.us = fadd fast float %add50.us.us.i.us, %mul42.us.us.i.us
  store float %add62.us.us.i.us, float* %i61.us.us.i.us, align 4, !tbaa !13
  %indvars.iv.next194.i.us = add nuw nsw i64 %indvars.iv193.i.us, 1
  %exitcond196.i.us = icmp eq i64 %indvars.iv.next194.i.us, %wide.trip.count.i
  br i1 %exitcond196.i.us, label %for.cond13.for.end67_crit_edge.us.us.i.us, label %for.body15.us.us.i.us

for.cond13.for.end67_crit_edge.us.us.i.us:        ; preds = %for.body15.us.us.i.us
  %indvars.iv.next198.i.us = add nsw i64 %indvars.iv197.i.us, %113
  %inc70.us.us.i.us = add nuw nsw i32 %q1.1136.us.us.i.us, 1
  %exitcond199.i.us = icmp eq i32 %inc70.us.us.i.us, %0
  br i1 %exitcond199.i.us, label %for.inc72.us.i.loopexit.us, label %for.body9.us.us.i.us

for.inc72.us.i.loopexit.us:                       ; preds = %for.cond13.for.end67_crit_edge.us.us.i.us
  %indvars.iv.next.i.us = add nuw nsw i64 %indvars.iv.i.us, 1
  %exitcond201.i.us = icmp eq i64 %indvars.iv.next.i.us, %wide.trip.count200.i
  br i1 %exitcond201.i.us, label %_ZL15kf_bfly_genericP12kiss_fft_cpxmP14kiss_fft_stateii.exit, label %for.cond2.preheader.us.i.us

for.cond2.preheader.us.i:                         ; preds = %for.cond2.preheader.us.preheader.i, %for.inc72.us.i
  %indvars.iv.i = phi i64 [ %indvars.iv.next.i, %for.inc72.us.i ], [ 0, %for.cond2.preheader.us.preheader.i ]
  br i1 %115, label %for.inc72.us.i.unr-lcssa, label %for.body4.us.i

for.inc72.us.i.unr-lcssa:                         ; preds = %for.body4.us.i, %for.cond2.preheader.us.i
  %indvars.iv188.i.unr = phi i64 [ 0, %for.cond2.preheader.us.i ], [ %indvars.iv.next189.i.3, %for.body4.us.i ]
  %indvars.iv186.i.unr = phi i64 [ %indvars.iv.i, %for.cond2.preheader.us.i ], [ %indvars.iv.next187.i.3, %for.body4.us.i ]
  br i1 %lcmp.mod, label %for.inc72.us.i, label %for.body4.us.i.epil

for.body4.us.i.epil:                              ; preds = %for.inc72.us.i.unr-lcssa, %for.body4.us.i.epil
  %indvars.iv188.i.epil = phi i64 [ %indvars.iv.next189.i.epil, %for.body4.us.i.epil ], [ %indvars.iv188.i.unr, %for.inc72.us.i.unr-lcssa ]
  %indvars.iv186.i.epil = phi i64 [ %indvars.iv.next187.i.epil, %for.body4.us.i.epil ], [ %indvars.iv186.i.unr, %for.inc72.us.i.unr-lcssa ]
  %epil.iter = phi i64 [ %epil.iter.sub, %for.body4.us.i.epil ], [ %xtraiter, %for.inc72.us.i.unr-lcssa ]
  %arrayidx.us.i.epil = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv186.i.epil
  %arrayidx6.us.i.epil = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv188.i.epil
  %143 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.epil to i64*
  %144 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.epil to i64*
  %145 = load i64, i64* %143, align 4
  store i64 %145, i64* %144, align 4
  %indvars.iv.next187.i.epil = add nsw i64 %indvars.iv186.i.epil, %113
  %indvars.iv.next189.i.epil = add nuw nsw i64 %indvars.iv188.i.epil, 1
  %epil.iter.sub = add i64 %epil.iter, -1
  %epil.iter.cmp = icmp eq i64 %epil.iter.sub, 0
  br i1 %epil.iter.cmp, label %for.inc72.us.i, label %for.body4.us.i.epil, !llvm.loop !18

for.inc72.us.i:                                   ; preds = %for.body4.us.i.epil, %for.inc72.us.i.unr-lcssa
  %146 = load i64, i64* %111, align 4
  %arrayidx12.us147.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.i
  %147 = bitcast %struct.kiss_fft_cpx* %arrayidx12.us147.i to i64*
  store i64 %146, i64* %147, align 4
  %indvars.iv.next.i = add nuw nsw i64 %indvars.iv.i, 1
  %exitcond201.i = icmp eq i64 %indvars.iv.next.i, %wide.trip.count200.i
  br i1 %exitcond201.i, label %_ZL15kf_bfly_genericP12kiss_fft_cpxmP14kiss_fft_stateii.exit, label %for.cond2.preheader.us.i

for.body4.us.i:                                   ; preds = %for.cond2.preheader.us.i, %for.body4.us.i
  %indvars.iv188.i = phi i64 [ %indvars.iv.next189.i.3, %for.body4.us.i ], [ 0, %for.cond2.preheader.us.i ]
  %indvars.iv186.i = phi i64 [ %indvars.iv.next187.i.3, %for.body4.us.i ], [ %indvars.iv.i, %for.cond2.preheader.us.i ]
  %niter = phi i64 [ %niter.nsub.3, %for.body4.us.i ], [ %unroll_iter, %for.cond2.preheader.us.i ]
  %arrayidx.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv186.i
  %arrayidx6.us.i = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv188.i
  %148 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i to i64*
  %149 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i to i64*
  %150 = load i64, i64* %148, align 4
  store i64 %150, i64* %149, align 4
  %indvars.iv.next187.i = add nsw i64 %indvars.iv186.i, %113
  %indvars.iv.next189.i = or i64 %indvars.iv188.i, 1
  %arrayidx.us.i.1 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.next187.i
  %arrayidx6.us.i.1 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv.next189.i
  %151 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.1 to i64*
  %152 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.1 to i64*
  %153 = load i64, i64* %151, align 4
  store i64 %153, i64* %152, align 4
  %indvars.iv.next187.i.1 = add nsw i64 %indvars.iv.next187.i, %113
  %indvars.iv.next189.i.1 = or i64 %indvars.iv188.i, 2
  %arrayidx.us.i.2 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.next187.i.1
  %arrayidx6.us.i.2 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv.next189.i.1
  %154 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.2 to i64*
  %155 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.2 to i64*
  %156 = load i64, i64* %154, align 4
  store i64 %156, i64* %155, align 4
  %indvars.iv.next187.i.2 = add nsw i64 %indvars.iv.next187.i.1, %113
  %indvars.iv.next189.i.2 = or i64 %indvars.iv188.i, 3
  %arrayidx.us.i.3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %Fout, i64 %indvars.iv.next187.i.2
  %arrayidx6.us.i.3 = getelementptr inbounds %struct.kiss_fft_cpx, %struct.kiss_fft_cpx* %110, i64 %indvars.iv.next189.i.2
  %157 = bitcast %struct.kiss_fft_cpx* %arrayidx.us.i.3 to i64*
  %158 = bitcast %struct.kiss_fft_cpx* %arrayidx6.us.i.3 to i64*
  %159 = load i64, i64* %157, align 4
  store i64 %159, i64* %158, align 4
  %indvars.iv.next187.i.3 = add nsw i64 %indvars.iv.next187.i.2, %113
  %indvars.iv.next189.i.3 = add nuw nsw i64 %indvars.iv188.i, 4
  %niter.nsub.3 = add i64 %niter, -4
  %niter.ncmp.3 = icmp eq i64 %niter.nsub.3, 0
  br i1 %niter.ncmp.3, label %for.inc72.us.i.unr-lcssa, label %for.body4.us.i

_ZL15kf_bfly_genericP12kiss_fft_cpxmP14kiss_fft_stateii.exit: ; preds = %for.inc72.us.i.loopexit.us, %for.inc72.us.i, %for.cond2.preheader.lr.ph.i, %sw.default
  tail call void @free(i8* %call.i) #7
  br label %sw.epilog

sw.epilog:                                        ; preds = %for.body.i, %do.body.i97, %do.body.us.i, %do.body.i80, %do.body.i, %sw.bb21, %_ZL15kf_bfly_genericP12kiss_fft_cpxmP14kiss_fft_stateii.exit
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i1 immarg) #4

; Function Attrs: nounwind
declare dso_local void @free(i8* nocapture) local_unnamed_addr #5

; Function Attrs: nounwind uwtable
define dso_local void @kiss_fft(%struct.kiss_fft_state* readonly %cfg, %struct.kiss_fft_cpx* %fin, %struct.kiss_fft_cpx* %fout) local_unnamed_addr #3 {
entry:
  %cmp.i = icmp eq %struct.kiss_fft_cpx* %fin, %fout
  br i1 %cmp.i, label %if.then.i, label %if.else.i

if.then.i:                                        ; preds = %entry
  %nfft.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %cfg, i64 0, i32 0
  %0 = load i32, i32* %nfft.i, align 4, !tbaa !6
  %conv.i = sext i32 %0 to i64
  %mul.i = shl nsw i64 %conv.i, 3
  %call.i = tail call noalias i8* @malloc(i64 %mul.i) #7
  %1 = bitcast i8* %call.i to %struct.kiss_fft_cpx*
  %arraydecay.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %cfg, i64 0, i32 2, i64 0
  tail call fastcc void @_ZL7kf_workP12kiss_fft_cpxPKS_miPiP14kiss_fft_state(%struct.kiss_fft_cpx* %1, %struct.kiss_fft_cpx* %fin, i64 1, i32 1, i32* nonnull %arraydecay.i, %struct.kiss_fft_state* %cfg) #7
  %2 = bitcast %struct.kiss_fft_cpx* %fin to i8*
  %3 = load i32, i32* %nfft.i, align 4, !tbaa !6
  %conv2.i = sext i32 %3 to i64
  %mul3.i = shl nsw i64 %conv2.i, 3
  tail call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %2, i8* align 4 %call.i, i64 %mul3.i, i1 false) #7
  tail call void @free(i8* %call.i) #7
  br label %kiss_fft_stride.exit

if.else.i:                                        ; preds = %entry
  %arraydecay5.i = getelementptr inbounds %struct.kiss_fft_state, %struct.kiss_fft_state* %cfg, i64 0, i32 2, i64 0
  tail call fastcc void @_ZL7kf_workP12kiss_fft_cpxPKS_miPiP14kiss_fft_state(%struct.kiss_fft_cpx* %fout, %struct.kiss_fft_cpx* %fin, i64 1, i32 1, i32* nonnull %arraydecay5.i, %struct.kiss_fft_state* %cfg) #7
  br label %kiss_fft_stride.exit

kiss_fft_stride.exit:                             ; preds = %if.then.i, %if.else.i
  ret void
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local void @kiss_fft_cleanup() local_unnamed_addr #6 {
entry:
  ret void
}

; Function Attrs: norecurse nounwind readnone uwtable
define dso_local i32 @kiss_fft_next_fast_size(i32 %n) local_unnamed_addr #6 {
entry:
  br label %while.body

while.body:                                       ; preds = %while.end14, %entry
  %n.addr.0 = phi i32 [ %n, %entry ], [ %n.addr.1, %while.end14 ]
  %rem2527 = and i32 %n.addr.0, 1
  %cmp28 = icmp eq i32 %rem2527, 0
  br i1 %cmp28, label %while.body2, label %while.cond3.preheader

while.cond3.preheader:                            ; preds = %while.body2, %while.body
  %m.0.lcssa = phi i32 [ %n.addr.0, %while.body ], [ %div, %while.body2 ]
  %rem430 = srem i32 %m.0.lcssa, 3
  %cmp531 = icmp eq i32 %rem430, 0
  br i1 %cmp531, label %while.body6, label %while.cond9.preheader

while.body2:                                      ; preds = %while.body, %while.body2
  %m.029 = phi i32 [ %div, %while.body2 ], [ %n.addr.0, %while.body ]
  %div = sdiv i32 %m.029, 2
  %rem25 = and i32 %div, 1
  %cmp = icmp eq i32 %rem25, 0
  br i1 %cmp, label %while.body2, label %while.cond3.preheader

while.cond9.preheader:                            ; preds = %while.body6, %while.cond3.preheader
  %m.1.lcssa = phi i32 [ %m.0.lcssa, %while.cond3.preheader ], [ %div7, %while.body6 ]
  %rem1034 = srem i32 %m.1.lcssa, 5
  %cmp1135 = icmp eq i32 %rem1034, 0
  br i1 %cmp1135, label %while.body12, label %while.end14

while.body6:                                      ; preds = %while.cond3.preheader, %while.body6
  %m.132 = phi i32 [ %div7, %while.body6 ], [ %m.0.lcssa, %while.cond3.preheader ]
  %div7 = sdiv i32 %m.132, 3
  %rem4 = srem i32 %div7, 3
  %cmp5 = icmp eq i32 %rem4, 0
  br i1 %cmp5, label %while.body6, label %while.cond9.preheader

while.body12:                                     ; preds = %while.cond9.preheader, %while.body12
  %m.236 = phi i32 [ %div13, %while.body12 ], [ %m.1.lcssa, %while.cond9.preheader ]
  %div13 = sdiv i32 %m.236, 5
  %rem10 = srem i32 %div13, 5
  %cmp11 = icmp eq i32 %rem10, 0
  br i1 %cmp11, label %while.body12, label %while.end14

while.end14:                                      ; preds = %while.body12, %while.cond9.preheader
  %m.2.lcssa = phi i32 [ %m.1.lcssa, %while.cond9.preheader ], [ %div13, %while.body12 ]
  %cmp15 = icmp slt i32 %m.2.lcssa, 2
  %not.cmp15 = xor i1 %cmp15, true
  %inc = zext i1 %not.cmp15 to i32
  %n.addr.1 = add nsw i32 %n.addr.0, %inc
  br i1 %cmp15, label %while.end16, label %while.body

while.end16:                                      ; preds = %while.end14
  ret i32 %n.addr.1
}

; Function Attrs: nounwind readnone speculatable
declare double @llvm.sqrt.f64(double) #2

; Function Attrs: nounwind readnone speculatable
declare double @llvm.floor.f64(double) #2

attributes #0 = { nofree nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #1 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #2 = { nounwind readnone speculatable }
attributes #3 = { nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }
attributes #5 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #6 = { norecurse nounwind readnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="false" "no-infs-fp-math"="true" "no-jump-tables"="false" "no-nans-fp-math"="true" "no-signed-zeros-fp-math"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="true" "use-soft-float"="false" }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 9.0.0 (https://gitlab.engr.illinois.edu/llvm/hpvm-release.git f83c355d11be7d011508763fedc67e76d592a636)"}
!2 = !{!3, !3, i64 0}
!3 = !{!"long", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C++ TBAA"}
!6 = !{!7, !8, i64 0}
!7 = !{!"_ZTS14kiss_fft_state", !8, i64 0, !8, i64 4, !4, i64 8, !4, i64 264}
!8 = !{!"int", !4, i64 0}
!9 = !{!7, !8, i64 4}
!10 = !{!11, !12, i64 0}
!11 = !{!"_ZTS12kiss_fft_cpx", !12, i64 0, !12, i64 4}
!12 = !{!"float", !4, i64 0}
!13 = !{!11, !12, i64 4}
!14 = !{!8, !8, i64 0}
!15 = distinct !{!15, !16}
!16 = !{!"llvm.loop.unroll.disable"}
!17 = distinct !{!17, !16}
!18 = distinct !{!18, !16}
